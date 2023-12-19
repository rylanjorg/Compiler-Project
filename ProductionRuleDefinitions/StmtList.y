StmtList: Stmt StmtList	
{ 
	if($1 != NULL) 
		$$ = createNode($1, $2, NODE_STMT);  
	else
		$$ = $2;
}
| Stmt 
{ 
	if($1 != NULL)
		$$ = $1; 
	else
		$$ = NULL; 

};
