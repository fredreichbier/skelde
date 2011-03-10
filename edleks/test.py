import os, subprocess

import edleks
from edleks.parser import parse
from edleks.compiler import CompilingVisitor

node = parse('''fubar := Object clone
fubar meh := "hi"

fubar hello := Method clone(
    meh println
    meh = "yo"
)
fubar hello
fubar hello
''')

visitor = CompilingVisitor()
compiled = visitor.visit(node)
compiled.print_me()

proc = subprocess.Popen([os.environ['SKELDE'], '-'], stdin=subprocess.PIPE)
print '--- [skelde] ---'
proc.communicate(compiled.generate_full())
