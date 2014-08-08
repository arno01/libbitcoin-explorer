/**
 * Copyright (c) 2011-2014 sx developers (see AUTHORS)
 *
 * This file is part of sx.
 *
 * sx is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <sx/command/send-tx.hpp>

#include <iostream>
#include <boost/format.hpp>
#include <bitcoin/bitcoin.hpp>
#include <obelisk/obelisk.hpp>
#include <sx/obelisk_client.hpp>
#include <sx/utility/config.hpp>
#include <sx/utility/utility.hpp>

using namespace bc;
using namespace sx;
using namespace sx::extension;

static void handle_callback(callback_args& args)
{
    args.output() << boost::format(SX_SEND_TX_OUTPUT) % now() << std::endl;
    args.stopped() = true;
}

console_result send_tx::invoke(std::ostream& output, std::ostream& cerr)
{
    // Bound parameters.
    const auto& transactions = get_transactions_argument();
    HANDLE_MULTIPLE_NOT_IMPLEMENTED(transactions);
    const transaction_type& tx = transactions.front();

    callback_args args(cerr, output);
    const auto handler = [&args](const std::error_code& error)
    {
        handle_error(args, error);
        handle_callback(args);
    };

    obelisk_client client(*this);
    auto& fullnode = client.get_fullnode();
    fullnode.protocol.broadcast_transaction(tx, handler);
    client.poll(args.stopped());

    return args.result();
}
