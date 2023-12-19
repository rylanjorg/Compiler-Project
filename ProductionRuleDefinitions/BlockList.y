BlockList: %empty
{
	$$ = NULL;
}
| BlockList StmtList
{
	$$ = createNode($1,$2, NODE_BLOCKLIST);
}
| BlockList VarDecl
{
	$$ = createNode($1,$2, NODE_BLOCKLIST);
};