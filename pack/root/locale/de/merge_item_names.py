import sys

def load_item_names(filepath, encoding="utf-8"):
    entries = {}
    order = []
    with open(filepath, "r", encoding=encoding) as f:
        for line in f:
            stripped = line.rstrip("\n")
            if "\t" in stripped:
                parts = stripped.split("\t", 1)
                try:
                    vnum = int(parts[0])
                    name = parts[1]
                    entries[vnum] = name
                    order.append(vnum)
                except ValueError:
                    order.append(stripped)
            else:
                order.append(stripped)
    return entries, order

def merge(de_file, en_file, output_file):
    de_entries, _ = load_item_names(de_file, encoding="latin-1")
    en_entries, en_order = load_item_names(en_file, encoding="latin-1")

    merged = 0
    with open(output_file, "w", encoding="utf-8") as out:
        for item in en_order:
            if isinstance(item, int):
                vnum = item
                if vnum in de_entries:
                    out.write(f"{vnum}\t{de_entries[vnum]}\n")
                    merged += 1
                else:
                    out.write(f"{vnum}\t{en_entries[vnum]}\n")
            else:
                out.write(item + "\n")

    print(f"Done. Merged {merged} translations from DE into output.")
    print(f"Output written to: {output_file}")

if __name__ == "__main__":
    if len(sys.argv) != 4:
        print("Usage: python merge_item_names.py item_names_de.txt item_names.txt output.txt")
        sys.exit(1)

    merge(sys.argv[1], sys.argv[2], sys.argv[3])