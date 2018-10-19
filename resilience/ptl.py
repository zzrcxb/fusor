import subprocess
import argparse
import re


def translate(source: str, codepage: str='utf-8', context: dict=None):
    string_pattern = re.compile(r'([ \t]*)#([^\r\n]+)')
    leading_ws = re.compile(r'^[ \t]*')

    HEAD = [
        '#!/usr/bin/python3',
        'import sys'
    ]

    if context:
        for name, value in context.items():
            HEAD.append('{} = {}'.format(name, repr(value)))

    lines = source.split('\n')
    for index, s in enumerate(lines):
        newline = not s.endswith("@")
        if not newline:
            s = s[:-1]
        m_obj = string_pattern.match(s)
        if m_obj:
            tmp_str = m_obj.group(2)
            cmt_obj = string_pattern.match(tmp_str)
            if not cmt_obj:
                l_ws = leading_ws.match(tmp_str).group(0).replace('\t', '    ')
                tmp_str = ' ' * (len(l_ws) // 4) * 4 + tmp_str.lstrip()
                indent = (len(m_obj.group(1).replace('\t', '    ')) // 4) * 4
                tmp_str = tmp_str.replace('{', '{{').replace('}', '}}').replace('{<', '').replace('>}', '').replace('\r', '').replace('\\#', '#').replace('"', '\\"')
                if newline:
                    tmp_str = ' ' * indent + 'sys.stdout.buffer.write("{raw}\\n".format(**locals()).encode("{cp}"))'.format(raw=tmp_str, cp=codepage)
                else:    
                  tmp_str = ' ' * indent + 'sys.stdout.buffer.write("{raw}".format(**locals()).encode("{cp}"))'.format(raw=tmp_str, cp=codepage)
            lines[index] = tmp_str
    p = subprocess.Popen(['python3', '-'], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
    stdout, stderr = p.communicate('\n'.join(HEAD + lines).encode(codepage))
    return stdout.decode(codepage)


if __name__ == '__main__':
    import sys
    import json

    parser = argparse.ArgumentParser()
    parser.add_argument('-c', '--codepage', type=str, help='Set codepage for your input file or strings and communication to python.', default='utf-8')
    parser.add_argument('-i', '--input', type=str, help='Set input file name.')
    parser.add_argument('-p', '--pipe', help='Get input from pipe, and it will override -i option', action='store_true')
    parser.add_argument('-o', '--output', type=str, help='output file name')
    parser.add_argument('--context', type=str, help='Specify context by passing a json string')
    args = parser.parse_args()

    if args.pipe:
        s = sys.stdin.buffer.read().decode(args.codepage)
    elif args.input:
        with open(args.input, 'r', encoding=args.codepage) as f:
            s = f.read()
    else:
        sys.stderr.write('An input file or pipe is required')
        exit(1)

    context = json.loads(args.context) if args.context else None
    res = translate(s, args.codepage, context)

    if args.output:
        with open(args.output, 'w', encoding=args.codepage) as f:
            f.write(res)
    else:
        sys.stdout.buffer.write(res.encode('utf-8'))
