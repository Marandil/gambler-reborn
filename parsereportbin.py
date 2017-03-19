import sys
import collections
import struct

def main():
    file_name = "fullreport.bin"
    if len(sys.argv) > 1:
        file_name = sys.argv[1]

    games = collections.Counter()
    won = collections.Counter()
    lengths = collections.Counter()

    headers = {}

    win_mask = 1 << 63
    len_mask = win_mask - 1

    with open(file_name, 'rb') as f:
        while True:
            blob = f.read(8)
            if not blob: break

            idx = struct.unpack("<Q", blob)[0]
            if idx not in headers:
                blob = f.read(64-8)
                header = blob[:blob.index(b'\x00')].decode()
                headers[idx] = header
            else:
                blob = f.read(8)
                length = struct.unpack("<Q", blob)[0]

                result = length & win_mask
                length = length & len_mask

                result = bool(result)

                games[idx] += 1
                if result:
                    won[idx] += 1
                lengths[idx] += length

    out_name = "parsedreport.csv"
    if len(sys.argv) > 2:
        out_name = sys.argv[2]

    with open(out_name, 'w') as f:
        f.write('sim;bs;i;N;p;q;runs;won;sumlengths;\n')
        for k in headers:
            f.write('{}{};{};{};\n'.format(headers[k],games[k],won[k],lengths[k]))

if __name__ == '__main__':
    main()
