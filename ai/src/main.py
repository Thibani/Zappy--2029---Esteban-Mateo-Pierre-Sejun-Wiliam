#!/usr/bin/env python3
"""
Zappy AI — Entry point.
Usage: ./zappy_ai -p <port> -n <team> -h <host>
"""

import argparse
import sys
from ai import ZappyAI

def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Zappy AI client")
    parser.add_argument("-p", type=int, required=True,  dest="port", help="Server port")
    parser.add_argument("-n", type=str, required=True,  dest="team", help="Team name")
    parser.add_argument("-H", type=str, default="localhost", dest="host", help="Server host")
    return parser.parse_args()


def main() -> None:
    args = parse_args()
    ai = ZappyAI(host=args.host, port=args.port, team=args.team)
    try:
        ai.run()
    except KeyboardInterrupt:
        pass
    except Exception as e:
        print(f"[AI] Fatal: {e}", file=sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    main()