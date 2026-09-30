#pragma once

#include "../Collection.hpp"

#include "../common/RequirementTypes.hpp"
#include "../../../libxml/Types.hpp"

namespace requirement {
    namespace xml {
        class Parser {
            public:
                using alignment_collection_t = std::vector<alignment>;
                using gold_collection_t = std::vector<gold>;
                using item_collection_t = std::vector<item>;

            public:
                Parser(const ::xml::Node& node);
                virtual ~Parser() = default;

                bool TryParse();
                void Parse();

                const ::xml::Node& GetNode() const { return node_; }

                alignment_collection_t Alignments() { return std::move(alignments_); }
                gold_collection_t Golds() { return std::move(golds_); }
                item_collection_t Items() { return std::move(items_); }

                Collection Collection();

            protected:
                void ParseAlignmentNode(const ::xml::Node& node, alignment& alignment);
                void ParseGoldNode(const ::xml::Node& node, gold& gold);
                void ParseItemNode(const ::xml::Node& node, item& item);

            protected:
                const ::xml::Node& node_;

                alignment_collection_t alignments_;
                gold_collection_t golds_;
                item_collection_t items_;
        };
    }  // namespace xml
}  // namespace requirement

