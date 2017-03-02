import sys
import collections

def main():
    file_name = "fullreport.csv"
    if len(sys.argv) > 1:
        file_name = sys.argv[1]

    games = collections.Counter()
    won = collections.Counter()
    lengths = collections.Counter()

    with open(file_name, 'r') as f:
        for line in f:
            # ignore sim; lines
            if 'sim;bs;i;N' in line:
                continue
            fields = line.split(';')
            key = ';'.join(fields[:6])

            games[key] += 1
            if fields[6] == 'true':
                won[key] += 1
            lengths[key] += int(fields[7])

    out_name = "parsedreport.csv"
    if len(sys.argv) > 2:
        out_name = sys.argv[2]

    with open(out_name, 'w') as f:
        f.write('sim;bs;i;N;p;q;runs;won;sumlengths;\n')
        for k in games:
            f.write('{};{};{};{};\n'.format(k,games[k],won[k],lengths[k]))

if __name__ == '__main__':
    main()
