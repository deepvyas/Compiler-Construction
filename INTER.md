* For Assignment statement:
	```
	child ==> var node ==> check type of var node
		if type==1 : only 1 child which holds tokenptr of id;
		if type==2 : two child first for for token of id and secons one 			     for token of index 
	child->sibling ==> expression

	assign value of expr to var node;
	```
* For moduleReuse statement:
	```
	child ==> idlist ==> check if child of idlist NULL;
	child->sibling ==> parameter_list ==>
	node->tokenptr ==> token of id of function to call;

	check retlist and idlist have smae size and 
	assign values respectively.
	```
