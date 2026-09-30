#!/usr/bin/env python3
"""
Script to merge Czech locale_game translations into English file based on left-side keys.
Preserves tab formatting and updates only matching keys.
"""

def detect_encoding(file_path):
    """Try to detect the file encoding."""
    encodings = ['utf-8', 'windows-1250', 'iso-8859-2', 'cp1252', 'latin1']
    
    for encoding in encodings:
        try:
            with open(file_path, 'r', encoding=encoding) as f:
                f.read()
            return encoding
        except (UnicodeDecodeError, UnicodeError):
            continue
    return 'utf-8'  # fallback


def merge_locale_translations(cz_file, en_file, output_file=None):
    """
    Merge Czech translations into English locale_game file based on matching keys.
    
    Args:
        cz_file: Path to Czech locale_game file
        en_file: Path to English locale_game file
        output_file: Path to output file (optional, defaults to en_file)
    """
    if output_file is None:
        output_file = en_file
    
    try:
        # Detect encodings
        cz_encoding = detect_encoding(cz_file)
        en_encoding = detect_encoding(en_file)
        print(f"Detected Czech file encoding: {cz_encoding}")
        print(f"Detected English file encoding: {en_encoding}")
        
        # Read Czech file and create KEY -> translation dictionary
        print(f"\nReading Czech translations from: {cz_file}")
        cz_translations = {}
        with open(cz_file, 'r', encoding=cz_encoding) as f:
            for line in f:
                line = line.rstrip('\n\r')  # Keep internal structure, remove only line breaks
                if line.strip() and '\t' in line:
                    parts = line.split('\t', 1)
                    key = parts[0].strip()
                    translation = parts[1] if len(parts) > 1 else ""
                    if key:  # Only add if key is not empty
                        cz_translations[key] = translation
        
        print(f"✓ Loaded {len(cz_translations)} Czech translations")
        
        # Read English file
        print(f"\nReading English file: {en_file}")
        with open(en_file, 'r', encoding=en_encoding) as f:
            en_lines = f.readlines()
        
        # Process English file and replace with Czech translations where available
        updated_lines = []
        replaced_count = 0
        kept_count = 0
        empty_lines = 0
        
        for line in en_lines:
            original_line = line.rstrip('\n\r')
            
            # Handle empty lines
            if not original_line.strip():
                updated_lines.append(line)
                empty_lines += 1
                continue
            
            # Check if line has tab separator
            if '\t' in original_line:
                parts = original_line.split('\t', 1)
                key = parts[0].strip()
                
                # Check if we have a Czech translation for this key
                if key and key in cz_translations:
                    # Replace with Czech translation, preserving tab structure
                    updated_lines.append(f"{key}\t{cz_translations[key]}\n")
                    replaced_count += 1
                else:
                    # Keep original English line
                    updated_lines.append(line)
                    kept_count += 1
            else:
                # Keep lines without tabs as-is
                updated_lines.append(line)
                kept_count += 1
        
        # Write merged file
        with open(output_file, 'w', encoding='utf-8') as f:
            f.writelines(updated_lines)
        
        print(f"\n✓ Merge completed successfully!")
        print(f"✓ Output saved to: {output_file}")
        print(f"✓ Lines replaced with Czech: {replaced_count}")
        print(f"✓ Lines kept in English: {kept_count}")
        print(f"✓ Empty/comment lines: {empty_lines}")
        print(f"✓ Total lines: {len(updated_lines)}")
        
        # Show Czech translations that weren't used
        used_keys = set()
        for line in en_lines:
            if '\t' in line:
                key = line.split('\t', 1)[0].strip()
                if key:
                    used_keys.add(key)
        
        unused = set(cz_translations.keys()) - used_keys
        if unused:
            print(f"\nℹ {len(unused)} Czech translations not used (keys not in English file)")
            if len(unused) <= 10:
                print(f"  Unused keys: {', '.join(sorted(unused))}")
            else:
                print(f"  First 10 unused keys: {', '.join(sorted(list(unused))[:10])}")
        
    except FileNotFoundError as e:
        print(f"Error: File not found - {e}")
    except Exception as e:
        print(f"Error: {e}")
        import traceback
        traceback.print_exc()


if __name__ == "__main__":
    import sys
    
    if len(sys.argv) < 3:
        print("Usage: python merge_locale.py <cz_file> <en_file> [output_file]")
        print("\nMerges Czech locale_game translations into English file based on matching keys.")
        print("\nExamples:")
        print("  python merge_locale.py locale_game_cz.txt locale_game_en.txt")
        print("  python merge_locale.py locale_game_cz.txt locale_game_en.txt merged_locale.txt")
        sys.exit(1)
    
    cz_file = sys.argv[1]
    en_file = sys.argv[2]
    output_file = sys.argv[3] if len(sys.argv) > 3 else None
    
    merge_locale_translations(cz_file, en_file, output_file)