import argparse, socket, time
def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--host", default="127.0.0.1")
    ap.add_argument("--port", type=int, default=5514)
    ap.add_argument("--events", type=int, default=50000)
    args = ap.parse_args()
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    start = time.time()
    msg = '<34>2025-09-30T00:00:00Z testhost app: benchmark event\n'
    for i in range(args.events):
        s.sendto(msg.encode(), (args.host, args.port))
    s.close()
    dur = time.time() - start
    eps = args.events/dur if dur>0 else 0
    print(f"Sent {args.events} events in {dur:.2f}s -> {eps:.0f} EPS")
if __name__ == "__main__":
    main()
