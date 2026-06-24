/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** test_client.cpp - Criterion tests for Client
*/
#include <criterion/criterion.h>
#include "server/client.hpp"

using namespace Zappy;

// -------------------------------------------------------------------------
// Command queue
// -------------------------------------------------------------------------

Test(client, command_queue_basic)
{
    Client client(42);
    client.feedReadBuffer("Forward\nRight\nLeft\n");

    std::string cmd;
    cr_assert(client.popCommand(cmd));
    cr_assert_str_eq(cmd.c_str(), "Forward");

    cr_assert(client.popCommand(cmd));
    cr_assert_str_eq(cmd.c_str(), "Right");

    cr_assert(client.popCommand(cmd));
    cr_assert_str_eq(cmd.c_str(), "Left");

    cr_assert_not(client.popCommand(cmd));
}

Test(client, partial_command)
{
    Client client(42);
    client.feedReadBuffer("Forw");

    std::string cmd;
    cr_assert_not(client.popCommand(cmd));

    client.feedReadBuffer("ard\n");
    cr_assert(client.popCommand(cmd));
    cr_assert_str_eq(cmd.c_str(), "Forward");
}

Test(client, crlf_stripping)
{
    Client client(42);
    client.feedReadBuffer("Forward\r\n");

    std::string cmd;
    cr_assert(client.popCommand(cmd));
    cr_assert_str_eq(cmd.c_str(), "Forward");
}

Test(client, empty_lines_ignored)
{
    Client client(42);
    client.feedReadBuffer("\n\n\nForward\n\n");

    std::string cmd;
    cr_assert(client.popCommand(cmd));
    cr_assert_str_eq(cmd.c_str(), "Forward");
    cr_assert_not(client.popCommand(cmd));
}

Test(client, queue_capped_at_max)
{
    Client client(42);
    std::string bulk;
    for (int i = 0; i < 15; ++i)
        bulk += "Forward\n";
    client.feedReadBuffer(bulk);

    int count = 0;
    std::string cmd;
    while (client.popCommand(cmd))
        ++count;

    cr_assert_eq(count, (int)Client::MAX_CMD_QUEUE);
}

Test(client, multiple_commands_one_feed)
{
    Client client(42);
    client.feedReadBuffer("Forward\nRight\n");

    std::string cmd;
    cr_assert_eq(client.commandQueueSize(), 2u);
    cr_assert(client.popCommand(cmd));
    cr_assert_eq(client.commandQueueSize(), 1u);
}

// -------------------------------------------------------------------------
// Write buffer
// -------------------------------------------------------------------------

Test(client, write_buffer_push_and_get)
{
    Client client(42);
    client.pushToWriteBuffer("ok\n");
    client.pushToWriteBuffer("hello\n");

    cr_assert_str_eq(client.getWriteBuffer().c_str(), "ok\nhello\n");
}

Test(client, write_buffer_consume_partial)
{
    Client client(42);
    client.pushToWriteBuffer("ok\nhello\n");
    client.consumeWriteBuffer(3); // consume "ok\n"

    cr_assert_str_eq(client.getWriteBuffer().c_str(), "hello\n");
}

Test(client, write_buffer_consume_all)
{
    Client client(42);
    client.pushToWriteBuffer("ok\n");
    client.consumeWriteBuffer(100);

    cr_assert(client.getWriteBuffer().empty());
}

Test(client, write_buffer_initially_empty)
{
    Client client(42);
    cr_assert(client.getWriteBuffer().empty());
}

// -------------------------------------------------------------------------
// State
// -------------------------------------------------------------------------

Test(client, initial_state)
{
    Client client(42);
    cr_assert_eq(client.getFd(), 42);
    cr_assert_eq(client.getType(), ClientType::UNKNOWN);
    cr_assert_not(client.isAuthenticated());
}

Test(client, set_type_and_auth)
{
    Client client(42);
    client.setType(ClientType::AI);
    client.setAuthenticated(true);

    cr_assert_eq(client.getType(), ClientType::AI);
    cr_assert(client.isAuthenticated());
}

Test(client, gui_type)
{
    Client client(42);
    client.setType(ClientType::GUI);

    cr_assert_eq(client.getType(), ClientType::GUI);
}