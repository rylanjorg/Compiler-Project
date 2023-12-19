
GlobalBlockList: %empty
{
	$$ = NULL;
}
| GlobalBlockList GlobVarDecl
{
	$$ = createNode($1,$2, NODE_BLOCKLIST);
}
| GlobalBlockList FuncDecl
{
	$$ = createNode($1,$2, NODE_BLOCKLIST);
};

