# Syntax Rules
## General

This language is staticly compiler and strongly typed. The only implemented types are TEXT, INT, & FLOAT. There are various statements that can be seen below

## Statements
<pre>
"PRINT" (expression | string) nl
"PUTCHAR" (expression | character) nl

---	
	
"IF" condition "THEN" nl 
	{statement}
["ELSE"
	{statement}]
"ENDIF" nl

---

"WHILE" condition "DO" nl
	{statement}
"ENDWHILE" nl

---

"LABEL" identifier nl
"GOTO" identifier nl

---

"INT" identifier "=" expression nl
"FLOAT" identifier "=" expression nl
"TEXT" identifier "=" expression | string nl
identifier "=" expression nl

---

"FUNC" [INT | FLOAT | TEXT |   ] identifier ["USING" (INT | FLOAT | TEXT) identifier {"," identifier}]"IS" nl 
	{statement} 
"ENDFUNC" nl

"DO" identifier ["WITH" expression {"," expression}]
</pre>

### Primitives
<pre>

program ::= 		{statement}

expression ::= term {("-" | "+") term}
term ::= unary {("*" | "/" | "%") unary}
unary ::= ["-" | "+"] primary
primary ::= number | identifier
condition ::= expression ((">" | ">=" | "<" | "<=" | "==") expression)+
nl ::= '\n'+

</pre>

### Built-in Functions

"LEN"(expression) : (TEXT) -> INT

"CHARAT"(expression, expression) : (TEXT, INT) -> INT

