#pragma once
#include <vector>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <mutex>
#include <shared_mutex>
#include <atomic>
#include "../EterBase/Singleton.h"
#include "Pack.h"
#include "PackSecurity.h"
#include <runtime_xorstr.hpp>

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

struct CacheEntry {
    uint64_t path_hash = 0;
    //PROTECTED_VALUE(uint64_t, path_hash, "agA7as5s5as")= 0;
    std::shared_ptr<const TPackFile> data = nullptr;
    uint32_t lru_counter = 0;
    uint32_t psl = 0;
};


namespace detail {
    struct TLCacheSlot {
        PROTECTED_VALUE_STATIC(uint64_t, hash, "hash---1111hash");
        //uint64_t hash;
        std::shared_ptr<const TPackFile> data;
    };
}

class CPackManager : public CSingleton<CPackManager> {
public:
    CPackManager();
    virtual ~CPackManager() = default;

    RX_FORCEINLINE size_t CPackManager::GetEntriesCount() const noexcept
    {
        size_t count = 0;
        {
            std::shared_lock lock(m_entries_mutex);
            count = m_entries.size();
            printf("[DEBUG] GetEntriesCount called, m_entries size: %zu\n", count);

            // Debug poszczególnych wpisów
            for (const auto& [hash, entry] : m_entries)
            {
                if (!entry.first) {
                    printf("  [WARNING] Entry hash 0x%llx has null pack pointer\n", hash);
                }
                else {
                    printf("  [DEBUG] Entry hash 0x%llx, pack %p, index %u\n",
                        hash,
                        entry.first.get(),
                        entry.second);
                }
            }
        }

        // Opcjonalnie debug cache table
        printf("[DEBUG] Cache table entries: %zu / %zu\n", m_cache_size.load(), m_cache_capacity.load());
        for (size_t i = 0; i < m_cache_table.size(); ++i)
        {
            const auto& e = m_cache_table[i];
            if (e.path_hash != 0) {
                printf("  [CACHE] slot %zu, hash 0x%llx, LRU %u, PSL %u, data %p\n",
                    i, e.path_hash, e.lru_counter, e.psl, e.data.get());
            }
        }

        return count;
    }
    RX_FORCEINLINE bool AddPack(const char* path) {
        rx::runtime_xorstr<MAX_PATH> encrypted_path(path);
        return AddPack_real(encrypted_path);
    }

    RX_FORCEINLINE std::shared_ptr<const TPackFile> GetFile(const char* path) {
        rx::runtime_xorstr<MAX_PATH> encrypted_path(path);
        return GetFile_real(encrypted_path);
    }

    RX_FORCEINLINE bool GetFile(const char* path, TPackFile& out_data) {
        rx::runtime_xorstr<MAX_PATH> encrypted_path(path);
        auto ptr = GetFile_real(encrypted_path);
        if (!ptr || ptr->empty()) {
            return false;
        }
        out_data = *ptr;
        return true;
    }

    RX_FORCEINLINE bool IsExist(const char* path) {
        rx::runtime_xorstr<MAX_PATH> encrypted_path(path);
        return IsExist_real(encrypted_path);
    }

    void ClearCache();

     void SetPackLoadMode() noexcept {
        m_load_from_pack.store(true, std::memory_order_relaxed);
    }

     void SetFileLoadMode() noexcept {
        m_load_from_pack.store(false, std::memory_order_relaxed);
    }


    void BeginParallelLoading() {
        m_parallel_mode.store(true, std::memory_order_release);
    }

    void EndParallelLoading() {
        m_parallel_mode.store(false, std::memory_order_release);
    }

    bool IsParallelMode() const {
        return m_parallel_mode.load(std::memory_order_acquire);
    }

private:

    RX_FORCEINLINE void NormalizePath_SIMD(const char* src, size_t src_len, char* dest) const noexcept;
    RX_FORCEINLINE void StripPathPrefix(const char* src, size_t src_len, const char*& out_src, size_t& out_len) const noexcept;
    RX_FORCEINLINE uint64_t ComputePathHash(const char* path, size_t len) const noexcept;



    bool AddPack_real(rx::runtime_xorstr<MAX_PATH>& path);
    std::shared_ptr<const TPackFile> GetFile_real(rx::runtime_xorstr<MAX_PATH>& path);
    bool IsExist_real(rx::runtime_xorstr<MAX_PATH>& path);

    RX_FORCEINLINE bool GetFileFromPack_real(uint64_t path_hash, std::shared_ptr<const TPackFile>& result);



    std::shared_ptr<const TPackFile> GetFromCache_real(uint64_t path_hash);
    void AddToCache_real(uint64_t path_hash, const std::shared_ptr<const TPackFile>& data);
    void EvictEntry();
    void GrowCache();

    RX_FORCEINLINE size_t FindSlot(uint64_t hash) const noexcept;
    RX_FORCEINLINE size_t FindInsertSlot(uint64_t hash, uint32_t& out_psl) noexcept;

private:


    std::atomic<bool> m_parallel_mode{ false };
    std::atomic<bool> m_load_from_pack{ true };



    TPackFileMap m_entries;


    mutable std::shared_mutex m_entries_mutex;

    PROTECTED_VALUE_STATIC(size_t, m_max_cache_memory, "m_max_cache_memory_ttt");
    //size_t m_max_cache_memory;
    std::atomic<size_t> m_current_cache_memory{ 0 };
    std::atomic<uint32_t> m_lru_clock{ 0 };


    std::vector<CacheEntry> m_cache_table;
    std::atomic<size_t> m_cache_capacity;
    std::atomic<size_t> m_cache_size{ 0 };

    static constexpr size_t INITIAL_CACHE_CAPACITY = 4096;
    static constexpr float MAX_LOAD_FACTOR = 0.75f;
    static constexpr size_t TL_CACHE_SIZE = 16; 

    static constexpr const char* PATH_PREFIX_1 = "d:/ymir work/";
    static constexpr size_t PATH_PREFIX_LEN = 13;
};


RX_FORCEINLINE void CPackManager::NormalizePath_SIMD(const char* src, size_t src_len, char* dest) const noexcept {
    extern const std::array<char, 256> TOLOWER_TABLE;

    size_t i = 0;

    const __m128i backslash = _mm_set1_epi8('\\');
    const __m128i slash = _mm_set1_epi8('/');
    const __m128i upper_a = _mm_set1_epi8('A' - 1);
    const __m128i upper_z = _mm_set1_epi8('Z' + 1);
    const __m128i to_lower = _mm_set1_epi8(32);

    for (; i + 16 <= src_len; i += 16) {
        __m128i chunk = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src + i));

        __m128i is_backslash = _mm_cmpeq_epi8(chunk, backslash);
        chunk = _mm_blendv_epi8(chunk, slash, is_backslash);

        __m128i is_upper = _mm_and_si128(
            _mm_cmpgt_epi8(chunk, upper_a),
            _mm_cmpgt_epi8(upper_z, chunk)
        );
        __m128i lowered = _mm_add_epi8(chunk, to_lower);
        chunk = _mm_blendv_epi8(chunk, lowered, is_upper);

        _mm_storeu_si128(reinterpret_cast<__m128i*>(dest + i), chunk);
    }

    for (; i < src_len; ++i) {
        dest[i] = (src[i] == '\\') ? '/' : TOLOWER_TABLE[static_cast<unsigned char>(src[i])];
    }
}
PACK_FORCEINLINE size_t kustom_strlen(const void* base_ptr, size_t max_scan)
{
    if (!base_ptr) return 0;

    const unsigned char* ptr = static_cast<const unsigned char*>(base_ptr);
    size_t offset = 0;
    const __m128i zero = _mm_setzero_si128();

    while (offset + 16 <= max_scan)
    {
        if ((reinterpret_cast<uintptr_t>(ptr + offset) & 0xFFF) > (4096 - 16)) {
            break;
        }

        __m128i chunk = _mm_loadu_si128(reinterpret_cast<const __m128i*>(ptr + offset));
        __m128i cmp = _mm_cmpeq_epi8(chunk, zero);
        int mask = _mm_movemask_epi8(cmp);

        if (mask != 0)
        {
            unsigned long index = 0;
#if defined(_MSC_VER)
            _BitScanForward(&index, static_cast<unsigned long>(mask));
#else
            index = static_cast<unsigned long>(__builtin_ctz(mask));
#endif
            return offset + index;
        }

        offset += 16;
    }
    while (offset < max_scan)
    {
        if (ptr[offset] == 0) return offset;
        ++offset;
    }

    return max_scan;
}

RX_FORCEINLINE void CPackManager::StripPathPrefix(const char* src, size_t src_len, const char*& out_src, size_t& out_len) const noexcept {
    extern const std::array<char, 256> TOLOWER_TABLE;

    out_src = src;
    out_len = src_len;

    if (src_len < PATH_PREFIX_LEN) return;

    bool match = true;
    for (size_t i = 0; i < PATH_PREFIX_LEN; ++i) {
        char a = src[i];
        char b = PATH_PREFIX_1[i];

        if (a == '\\') a = '/';
        if (b == '\\') b = '/';

        if (TOLOWER_TABLE[static_cast<unsigned char>(a)] !=
            TOLOWER_TABLE[static_cast<unsigned char>(b)]) {
            match = false;
            break;
        }
    }

    if (match) {
        out_src = src + PATH_PREFIX_LEN;
        out_len = src_len - PATH_PREFIX_LEN;
    }
}

RX_FORCEINLINE uint64_t CPackManager::ComputePathHash(const char* path, size_t len) const noexcept {
    const uint64_t raw_hash = XXH3_64bits_withSeed(path, len, PackSecurity::XXH3_SALT);
    return PackSecurity::obfuscate_hash(raw_hash);
}

RX_FORCEINLINE size_t CPackManager::FindSlot(uint64_t hash) const noexcept {
    const size_t capacity = m_cache_capacity.load(std::memory_order_relaxed);
    size_t slot = hash & (capacity - 1);
    uint32_t psl = 0;

    while (true) {
        const CacheEntry& entry = m_cache_table[slot];

        if (entry.path_hash == 0) {
            return capacity;
        }

        if (entry.path_hash == hash) {
            return slot;
        }

        if (psl > entry.psl) {
            return capacity;
        }

        slot = (slot + 1) & (capacity - 1);
        ++psl;
    }
}

RX_FORCEINLINE size_t CPackManager::FindInsertSlot(uint64_t hash, uint32_t& out_psl) noexcept {
    const size_t capacity = m_cache_capacity.load(std::memory_order_relaxed);
    size_t slot = hash & (capacity - 1);
    out_psl = 0;

    while (true) {
        CacheEntry& entry = m_cache_table[slot];

        if (entry.path_hash == 0) {
            return slot;
        }

        if (entry.path_hash == hash) {
            return slot;
        }

        slot = (slot + 1) & (capacity - 1);
        ++out_psl;
    }
}

RX_FORCEINLINE bool CPackManager::GetFileFromPack_real(uint64_t path_hash, std::shared_ptr<const TPackFile>& result) {
    auto it = m_entries.find(path_hash);
    if (it != m_entries.end()) {
        return it->second.first->GetFile(it->second.second, result);
    }
    return false;
}