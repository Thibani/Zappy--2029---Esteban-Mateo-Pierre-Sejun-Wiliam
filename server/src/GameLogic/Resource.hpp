#pragma once
#include <string>

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