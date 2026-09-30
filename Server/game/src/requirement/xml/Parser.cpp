#include "../../stdafx.h"
#include "Parser.hpp"

#include "../../../libxml/xml.hpp"

namespace requirement {
    namespace xml {
        Parser::Parser(const ::xml::Node& node) : node_(node) {}

        bool Parser::TryParse() {
            try {
                Parse();
            } catch (const std::exception& e) {
                sys_err(e.what());
                return false;
            }

            return true;
        }

        void Parser::Parse() {
            for (const auto& node : ::xml::IterableNode(node_)) {
                const auto& name = ::xml::GetName(node);
                if (name == "Alignment") {
                    alignment alignment;
                    ParseAlignmentNode(node, alignment);
                    alignments_.emplace_back(std::move(alignment));
                } else if (name == "Gold") {
                    gold gold;
                    ParseGoldNode(node, gold);
                    golds_.emplace_back(std::move(gold));
                } else if (name == "Item") {
                    item item;
                    ParseItemNode(node, item);
                    items_.emplace_back(std::move(item));
                }
            }
        }

        Collection Parser::Collection() {
            return requirement::Collection(*this);
        }

        void Parser::ParseAlignmentNode(const ::xml::Node& node, alignment& alignment) {
            ::xml::GetAttribute(node, "amount", 0, alignment.amount);
        }

        void Parser::ParseGoldNode(const ::xml::Node& node, gold& gold) {
            ::xml::GetAttribute<decltype(gold.amount)>(node, "amount", 0, gold.amount);
        }

        void Parser::ParseItemNode(const ::xml::Node& node, item& item) {
            ::xml::GetAttribute(node, "vnum", item.vnum);
            ::xml::GetAttribute<decltype(item.count)>(node, "count", 1, item.count);
        }
    }  // namespace xml
}  // namespace requirement
