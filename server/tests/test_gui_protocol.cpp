/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** test_gui_protocol.cpp - Criterion tests for GUIProtocol formatters.
**
** Tests only the static fmt*() methods (pure formatters) — these are the
** ones that matter for spec correctness. They don't require Server or
** any other dependency to compile.
**
** Note: each formatter result is stored in a named std::string before
** calling cr_assert_str_eq, because .c_str() on a temporary returns a
** pointer that may be invalidated before Criterion reads it (non-SSO
** strings allocate on heap and free immediately when the temporary dies).
*/

#include <criterion/criterion.h>
#include <string>
#include <vector>

#include "protocol/guiProtocol.hpp"

using namespace Zappy;

// Helper: compare a formatter output against an expected string.
#define EXPECT_FMT(actual_expr, expected) do {              \
    std::string _actual = (actual_expr);                    \
    cr_assert_str_eq(_actual.c_str(), (expected),           \
        "expected '%s' got '%s'", (expected), _actual.c_str()); \
} while (0)

// -------------------------------------------------------------------------
// Map / world messages
// -------------------------------------------------------------------------

Test(gui_protocol, fmt_msz)
{
    EXPECT_FMT(GUIProtocol::fmtMsz(10, 10), "msz 10 10\n");
    EXPECT_FMT(GUIProtocol::fmtMsz(42, 7),  "msz 42 7\n");
}

Test(gui_protocol, fmt_bct_empty_tile)
{
    Inventory empty;
    EXPECT_FMT(GUIProtocol::fmtBct(0, 0, empty), "bct 0 0 0 0 0 0 0 0 0\n");
}

Test(gui_protocol, fmt_bct_with_resources)
{
    Inventory inv;
    inv[Resource::FOOD]     = 3;
    inv[Resource::LINEMATE] = 1;
    inv[Resource::THYSTAME] = 1;
    EXPECT_FMT(GUIProtocol::fmtBct(4, 5, inv), "bct 4 5 3 1 0 0 0 0 1\n");
}

Test(gui_protocol, fmt_tna)
{
    EXPECT_FMT(GUIProtocol::fmtTna("Trantor"), "tna Trantor\n");
    EXPECT_FMT(GUIProtocol::fmtTna("team_with_underscore"),
               "tna team_with_underscore\n");
}

// -------------------------------------------------------------------------
// Player messages
// -------------------------------------------------------------------------

Test(gui_protocol, fmt_pnw)
{
    EXPECT_FMT(GUIProtocol::fmtPnw(1, 4, 5, Orientation::EAST, 1, "Trantor"),
               "pnw #1 4 5 2 1 Trantor\n");
}

Test(gui_protocol, fmt_pnw_all_orientations)
{
    EXPECT_FMT(GUIProtocol::fmtPnw(1, 0, 0, Orientation::NORTH, 1, "T"),
               "pnw #1 0 0 1 1 T\n");
    EXPECT_FMT(GUIProtocol::fmtPnw(1, 0, 0, Orientation::EAST, 1, "T"),
               "pnw #1 0 0 2 1 T\n");
    EXPECT_FMT(GUIProtocol::fmtPnw(1, 0, 0, Orientation::SOUTH, 1, "T"),
               "pnw #1 0 0 3 1 T\n");
    EXPECT_FMT(GUIProtocol::fmtPnw(1, 0, 0, Orientation::WEST, 1, "T"),
               "pnw #1 0 0 4 1 T\n");
}

Test(gui_protocol, fmt_ppo)
{
    EXPECT_FMT(GUIProtocol::fmtPpo(7, 12, 8, Orientation::SOUTH),
               "ppo #7 12 8 3\n");
}

Test(gui_protocol, fmt_plv)
{
    EXPECT_FMT(GUIProtocol::fmtPlv(3, 5),   "plv #3 5\n");
    EXPECT_FMT(GUIProtocol::fmtPlv(99, 8),  "plv #99 8\n");
}

Test(gui_protocol, fmt_pin)
{
    Inventory inv;
    inv[Resource::FOOD]     = 10;
    inv[Resource::LINEMATE] = 1;
    EXPECT_FMT(GUIProtocol::fmtPin(1, 5, 5, inv),
               "pin #1 5 5 10 1 0 0 0 0 0\n");
}

Test(gui_protocol, fmt_pex)
{
    EXPECT_FMT(GUIProtocol::fmtPex(2), "pex #2\n");
}

Test(gui_protocol, fmt_pbc)
{
    EXPECT_FMT(GUIProtocol::fmtPbc(1, "rally on me"),
               "pbc #1 rally on me\n");
    EXPECT_FMT(GUIProtocol::fmtPbc(42, ""), "pbc #42 \n");
}

Test(gui_protocol, fmt_pfk)
{
    EXPECT_FMT(GUIProtocol::fmtPfk(1), "pfk #1\n");
}

Test(gui_protocol, fmt_pdi)
{
    EXPECT_FMT(GUIProtocol::fmtPdi(7), "pdi #7\n");
}

// -------------------------------------------------------------------------
// Resource pick up / drop
// -------------------------------------------------------------------------

Test(gui_protocol, fmt_pgt)
{
    EXPECT_FMT(GUIProtocol::fmtPgt(1, Resource::FOOD),     "pgt #1 0\n");
    EXPECT_FMT(GUIProtocol::fmtPgt(1, Resource::LINEMATE), "pgt #1 1\n");
    EXPECT_FMT(GUIProtocol::fmtPgt(1, Resource::THYSTAME), "pgt #1 6\n");
}

Test(gui_protocol, fmt_pdr)
{
    EXPECT_FMT(GUIProtocol::fmtPdr(2, Resource::SIBUR), "pdr #2 3\n");
}

// -------------------------------------------------------------------------
// Incantation
// -------------------------------------------------------------------------

Test(gui_protocol, fmt_pic_single_player)
{
    std::vector<int> ids{1};
    EXPECT_FMT(GUIProtocol::fmtPic(5, 5, 1, ids), "pic 5 5 1 #1\n");
}

Test(gui_protocol, fmt_pic_multiple_players)
{
    std::vector<int> ids{1, 3, 5, 7};
    EXPECT_FMT(GUIProtocol::fmtPic(5, 5, 4, ids),
               "pic 5 5 4 #1 #3 #5 #7\n");
}

Test(gui_protocol, fmt_pie_success)
{
    EXPECT_FMT(GUIProtocol::fmtPie(5, 5, true), "pie 5 5 1\n");
}

Test(gui_protocol, fmt_pie_failure)
{
    EXPECT_FMT(GUIProtocol::fmtPie(2, 2, false), "pie 2 2 0\n");
}

// -------------------------------------------------------------------------
// Eggs
// -------------------------------------------------------------------------

Test(gui_protocol, fmt_enw)
{
    EXPECT_FMT(GUIProtocol::fmtEnw(42, 1, 5, 5), "enw #42 #1 5 5\n");
}

Test(gui_protocol, fmt_ebo)
{
    EXPECT_FMT(GUIProtocol::fmtEbo(42), "ebo #42\n");
}

Test(gui_protocol, fmt_edi)
{
    EXPECT_FMT(GUIProtocol::fmtEdi(43), "edi #43\n");
}

// -------------------------------------------------------------------------
// Server-level
// -------------------------------------------------------------------------

Test(gui_protocol, fmt_sgt)
{
    EXPECT_FMT(GUIProtocol::fmtSgt(100), "sgt 100\n");
}

Test(gui_protocol, fmt_sst)
{
    EXPECT_FMT(GUIProtocol::fmtSst(200), "sst 200\n");
}

Test(gui_protocol, fmt_seg)
{
    EXPECT_FMT(GUIProtocol::fmtSeg("Trantor"), "seg Trantor\n");
}

Test(gui_protocol, fmt_smg)
{
    EXPECT_FMT(GUIProtocol::fmtSmg("server restarting"),
               "smg server restarting\n");
}

// -------------------------------------------------------------------------
// Enum values match protocol indices
// -------------------------------------------------------------------------

Test(gui_protocol, resource_indices_match_protocol)
{
    cr_assert_eq(static_cast<int>(Resource::FOOD),      0);
    cr_assert_eq(static_cast<int>(Resource::LINEMATE),  1);
    cr_assert_eq(static_cast<int>(Resource::DERAUMERE), 2);
    cr_assert_eq(static_cast<int>(Resource::SIBUR),     3);
    cr_assert_eq(static_cast<int>(Resource::MENDIANE),  4);
    cr_assert_eq(static_cast<int>(Resource::PHIRAS),    5);
    cr_assert_eq(static_cast<int>(Resource::THYSTAME),  6);
}

Test(gui_protocol, orientation_indices_match_protocol)
{
    cr_assert_eq(static_cast<int>(Orientation::NORTH), 1);
    cr_assert_eq(static_cast<int>(Orientation::EAST),  2);
    cr_assert_eq(static_cast<int>(Orientation::SOUTH), 3);
    cr_assert_eq(static_cast<int>(Orientation::WEST),  4);
}
