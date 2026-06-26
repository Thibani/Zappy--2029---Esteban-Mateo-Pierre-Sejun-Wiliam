/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** test_gui_command_handler.cpp - Criterion tests for GUICommandHandler
** parsing helpers.
**
** Tests only the static helpers (_parseInts, _parsePlayerId) — these are
** the pure functions that matter for parser correctness. They don't need
** GUIProtocol, Server, or any other dependency to compile.
**
** Behavior tests of the per-command handlers (_handleMsz, _handleBct...)
** are covered by the end-to-end netcat tests, since they're mostly
** dispatch + emit calls to GUIProtocol.
*/

#include <criterion/criterion.h>
#include <string>
#include <vector>

#include "server/guiCommandHandler.hpp"

using namespace Zappy;

// =========================================================================
// _parseInts
// =========================================================================

Test(parse_ints, one_int_ok)
{
    std::vector<int> out;
    cr_assert(GUICommandHandler::parseInts("42", out, 1));
    cr_assert_eq(out.size(), 1u);
    cr_assert_eq(out[0], 42);
}

Test(parse_ints, two_ints_ok)
{
    std::vector<int> out;
    cr_assert(GUICommandHandler::parseInts("3 4", out, 2));
    cr_assert_eq(out.size(), 2u);
    cr_assert_eq(out[0], 3);
    cr_assert_eq(out[1], 4);
}

Test(parse_ints, three_ints_ok)
{
    std::vector<int> out;
    cr_assert(GUICommandHandler::parseInts("1 2 3", out, 3));
    cr_assert_eq(out.size(), 3u);
    cr_assert_eq(out[0], 1);
    cr_assert_eq(out[1], 2);
    cr_assert_eq(out[2], 3);
}

Test(parse_ints, negative_numbers_accepted)
{
    std::vector<int> out;
    cr_assert(GUICommandHandler::parseInts("-5 -10", out, 2));
    cr_assert_eq(out[0], -5);
    cr_assert_eq(out[1], -10);
}

Test(parse_ints, zero_accepted)
{
    std::vector<int> out;
    cr_assert(GUICommandHandler::parseInts("0 0", out, 2));
    cr_assert_eq(out[0], 0);
    cr_assert_eq(out[1], 0);
}

Test(parse_ints, multiple_spaces_accepted)
{
    std::vector<int> out;
    cr_assert(GUICommandHandler::parseInts("3    4", out, 2));
    cr_assert_eq(out[0], 3);
    cr_assert_eq(out[1], 4);
}

Test(parse_ints, leading_whitespace_accepted)
{
    std::vector<int> out;
    cr_assert(GUICommandHandler::parseInts("   3 4", out, 2));
    cr_assert_eq(out[0], 3);
    cr_assert_eq(out[1], 4);
}

Test(parse_ints, too_few_ints_rejected)
{
    std::vector<int> out;
    cr_assert_not(GUICommandHandler::parseInts("3", out, 2));
}

Test(parse_ints, too_many_ints_rejected)
{
    std::vector<int> out;
    cr_assert_not(GUICommandHandler::parseInts("1 2 3", out, 2));
}

Test(parse_ints, empty_string_rejected_when_expected)
{
    std::vector<int> out;
    cr_assert_not(GUICommandHandler::parseInts("", out, 1));
}

Test(parse_ints, empty_string_accepted_when_zero_expected)
{
    std::vector<int> out;
    cr_assert(GUICommandHandler::parseInts("", out, 0));
    cr_assert_eq(out.size(), 0u);
}

Test(parse_ints, non_integer_rejected)
{
    std::vector<int> out;
    cr_assert_not(GUICommandHandler::parseInts("abc def", out, 2));
}

Test(parse_ints, mixed_int_and_garbage_rejected)
{
    std::vector<int> out;
    cr_assert_not(GUICommandHandler::parseInts("3 abc", out, 2));
}

Test(parse_ints, trailing_garbage_rejected)
{
    std::vector<int> out;
    cr_assert_not(GUICommandHandler::parseInts("5 5 garbage", out, 2));
}

// =========================================================================
// _parsePlayerId
// =========================================================================

Test(parse_player_id, simple_id_ok)
{
    int id = -1;
    cr_assert(GUICommandHandler::parsePlayerId("#1", id));
    cr_assert_eq(id, 1);
}

Test(parse_player_id, two_digit_id_ok)
{
    int id = -1;
    cr_assert(GUICommandHandler::parsePlayerId("#42", id));
    cr_assert_eq(id, 42);
}

Test(parse_player_id, large_id_ok)
{
    int id = -1;
    cr_assert(GUICommandHandler::parsePlayerId("#999", id));
    cr_assert_eq(id, 999);
}

Test(parse_player_id, zero_id_ok)
{
    int id = -1;
    cr_assert(GUICommandHandler::parsePlayerId("#0", id));
    cr_assert_eq(id, 0);
}

Test(parse_player_id, missing_hash_rejected)
{
    int id = -1;
    cr_assert_not(GUICommandHandler::parsePlayerId("1", id));
}

Test(parse_player_id, only_hash_rejected)
{
    int id = -1;
    cr_assert_not(GUICommandHandler::parsePlayerId("#", id));
}

Test(parse_player_id, hash_with_letters_rejected)
{
    int id = -1;
    cr_assert_not(GUICommandHandler::parsePlayerId("#abc", id));
}

Test(parse_player_id, mixed_digits_and_letters_rejected)
{
    int id = -1;
    cr_assert_not(GUICommandHandler::parsePlayerId("#1a2", id));
}

Test(parse_player_id, negative_id_rejected)
{
    int id = -1;
    cr_assert_not(GUICommandHandler::parsePlayerId("#-5", id));
}

Test(parse_player_id, empty_string_rejected)
{
    int id = -1;
    cr_assert_not(GUICommandHandler::parsePlayerId("", id));
}