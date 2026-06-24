#pragma once
#include <string>
#include <vector>

namespace Zappy {

    enum TypeResource {
        FOOD,
        LINEMATE,
        DERAUMERE,
        SIBUR,
        MENDIANE,
        PHIRAS,
        THYSTAME
    };

    constexpr int kResourceCount = 7;

    class Inventory {
        public:
            Inventory() : _counts(kResourceCount, 0) {}

            int get(TypeResource r) const { return _counts[r]; }
            bool has(TypeResource r) const { return _counts[r] > 0; }

            void set(TypeResource r, int n) { _counts[r] = n; }
            void add(TypeResource r, int n = 1) { _counts[r] += n; }
            void remove(TypeResource r, int n = 1) { _counts[r] -= n; }

            const std::vector<int> &raw() const { return _counts; }
            std::vector<int> &raw() { return _counts; }

        private:
            std::vector<int> _counts;
    };

    class Resource {

        public:
            static std::string typeResourceToString(TypeResource typeResource)
            {
                switch (typeResource) {
                    case FOOD:
                        return "food";
                        break;
                    case LINEMATE:
                        return "linemate";
                        break;
                    case DERAUMERE:
                        return "debaumere";
                        break;
                    case SIBUR:
                        return "sibur";
                        break;
                    case MENDIANE:
                        return "mendiane";
                        break;
                    case PHIRAS:
                        return "phiras";
                        break;
                    case THYSTAME:
                        return "thystame";
                        break;
                }
                return "";
            }
    };
}