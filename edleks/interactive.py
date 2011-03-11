import os, subprocess

import edleks
from edleks.parser import parse
from edleks.compiler import CompilingVisitor

visitor = CompilingVisitor()

proc = subprocess.Popen([os.environ['SKELDE'], '-'], stdin=subprocess.PIPE)
while True:
    try:
        node = parse(raw_input('edleks> '))
        msg = visitor.visit(node)
    except Exception, e:
        print '[edleks] error: %s' % e
        continue
    proc.stdin.write(msg.generate_full())
    raw_input()
