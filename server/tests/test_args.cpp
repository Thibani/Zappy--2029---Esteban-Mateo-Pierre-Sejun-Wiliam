/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** test_args.cpp - Criterion tests for ArgsParser
*/
#include <criterion/criterion.h>
#include <criterion/redirect.h>
#include "utils/args.hpp"
#include "exceptions/serverException.hpp"

using namespace Zappy;

static std::pair<int, std::vector<char *>> makeArgv(std::vector<std::string> &args)
{
    std::vector<char *> argv;
    for (auto &s : args)
        argv.push_back(s.data());
    return {static_cast<int>(argv.size()), argv};
}

// -------------------------------------------------------------------------
// Valid cases
// -------------------------------------------------------------------------

Test(args, valid_minimal)
{
    std::vector<std::string> raw = {
        "./zappy_server", "-p", "4242", "-x", "10", "-y", "10",
        "-n", "team1", "-c", "5"
    };
    auto [argc, argv] = makeArgv(raw);
    Args args = ArgsParser::parse(argc, argv.data());

    cr_assert_eq(args.port,      4242);
    cr_assert_eq(args.width,     10);
    cr_assert_eq(args.height,    10);
    cr_assert_eq(args.clientsNb, 5);
    cr_assert_eq(args.freq,      100);
    cr_assert_eq(args.teams.size(), 1u);
    cr_assert_str_eq(args.teams[0].c_str(), "team1");
}

Test(args, valid_multiple_teams)
{
    std::vector<std::string> raw = {
        "./zappy_server", "-p", "4242", "-x", "10", "-y", "10",
        "-n", "team1", "team2", "team3", "-c", "5", "-f", "50"
    };
    auto [argc, argv] = makeArgv(raw);
    Args args = ArgsParser::parse(argc, argv.data());

    cr_assert_eq(args.teams.size(), 3u);
    cr_assert_eq(args.freq, 50);
}

Test(args, valid_freq_default)
{
    std::vector<std::string> raw = {
        "./zappy_server", "-p", "1234", "-x", "20", "-y", "20",
        "-n", "alpha", "-c", "2"
    };
    auto [argc, argv] = makeArgv(raw);
    Args args = ArgsParser::parse(argc, argv.data());

    cr_assert_eq(args.freq, 100);
}

// -------------------------------------------------------------------------
// Invalid cases
// -------------------------------------------------------------------------

Test(args, missing_port)
{
    std::vector<std::string> raw = {
        "./zappy_server", "-x", "10", "-y", "10", "-n", "team1", "-c", "5"
    };
    auto [argc, argv] = makeArgv(raw);
    cr_assert_throw(ArgsParser::parse(argc, argv.data()), ArgsException);
}

Test(args, invalid_port_zero)
{
    std::vector<std::string> raw = {
        "./zappy_server", "-p", "0", "-x", "10", "-y", "10", "-n", "team1", "-c", "5"
    };
    auto [argc, argv] = makeArgv(raw);
    cr_assert_throw(ArgsParser::parse(argc, argv.data()), ArgsException);
}

Test(args, invalid_port_too_high)
{
    std::vector<std::string> raw = {
        "./zappy_server", "-p", "99999", "-x", "10", "-y", "10", "-n", "team1", "-c", "5"
    };
    auto [argc, argv] = makeArgv(raw);
    cr_assert_throw(ArgsParser::parse(argc, argv.data()), ArgsException);
}

Test(args, reserved_team_name_graphic)
{
    std::vector<std::string> raw = {
        "./zappy_server", "-p", "4242", "-x", "10", "-y", "10",
        "-n", "GRAPHIC", "-c", "5"
    };
    auto [argc, argv] = makeArgv(raw);
    cr_assert_throw(ArgsParser::parse(argc, argv.data()), ArgsException);
}

Test(args, missing_team_name)
{
    std::vector<std::string> raw = {
        "./zappy_server", "-p", "4242", "-x", "10", "-y", "10", "-c", "5"
    };
    auto [argc, argv] = makeArgv(raw);
    cr_assert_throw(ArgsParser::parse(argc, argv.data()), ArgsException);
}

Test(args, negative_width)
{
    std::vector<std::string> raw = {
        "./zappy_server", "-p", "4242", "-x", "-5", "-y", "10", "-n", "team1", "-c", "5"
    };
    auto [argc, argv] = makeArgv(raw);
    cr_assert_throw(ArgsParser::parse(argc, argv.data()), ArgsException);
}

Test(args, unknown_flag)
{
    std::vector<std::string> raw = {
        "./zappy_server", "-p", "4242", "-x", "10", "-y", "10",
        "-n", "team1", "-c", "5", "-z", "unknown"
    };
    auto [argc, argv] = makeArgv(raw);
    cr_assert_throw(ArgsParser::parse(argc, argv.data()), ArgsException);
}

Test(args, missing_width)
{
    std::vector<std::string> raw = {
        "./zappy_server", "-p", "4242", "-y", "10", "-n", "team1", "-c", "5"
    };
    auto [argc, argv] = makeArgv(raw);
    cr_assert_throw(ArgsParser::parse(argc, argv.data()), ArgsException);
}

Test(args, missing_height)
{
    std::vector<std::string> raw = {
        "./zappy_server", "-p", "4242", "-x", "10", "-n", "team1", "-c", "5"
    };
    auto [argc, argv] = makeArgv(raw);
    cr_assert_throw(ArgsParser::parse(argc, argv.data()), ArgsException);
}