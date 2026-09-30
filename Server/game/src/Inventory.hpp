#pragma once


class CHARACTER;
class CItem;

class Inventory {

public:
	enum class EStatus {
		ERROR_UNDEFINED = -1,
		SUCCESS = 0,
		ERROR_NO_SPACE = 1,
	};


public:
    Inventory(CHARACTER& owner);
    virtual ~Inventory() = default;

    EStatus Add(CItem*& item);

    CItem* Get(const uint16_t& cell) const;

    bool FindEmptyPosition(CItem& item, uint16_t& cell) const;

    void Sort();
    void SortStack();

    inline CHARACTER& GetOwner() const { return owner_; }
    inline CHARACTER* GetOwnerPtr() const { return &GetOwner(); }

protected:
    bool Stack(CItem*& item);
    void TryStack(CItem& item,
                  uint16_t startCell,
                  uint16_t endCell,
                  uint16_t& count,
                  CItem*& match);
    CItem* FindStackableItem(CItem& originItem,
                             uint16_t startCell,
                             uint16_t endCell) const;

protected:
    CHARACTER& owner_;
};

