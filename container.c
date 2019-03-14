#include "9cc.h"


Map *new_map(){
	Map *map = malloc(sizeof(Map));
	map->keys = new_vector();
	map->vals = new_vector();
	return map;
}

void map_put(Map *map, char *key, void *val){
	vec_push(map->keys, key);
	vec_push(map->vals, val);
}

void *map_get(Map *map,char *key){
	for(int i=map->keys->len - 1;i>=0;i--)
		if(strcmp(map->keys->data[i],key)==0)
			return map->vals->data[i];
	return NULL;
}

void expect(int line, int expected, int actual){
	if(expected == actual)return;
	fprintf(stderr,"%d: %d expected, but got %d\n",line,expected,actual);
	exit(1);
}

void runtest(){
	Map *map = new_map();
	expect(__LINE__,100,(int)map_get(map,"foo"));

	map_put(map,"foo",(void *)2);
	expect(__LINE__,0,(int)map_get(map,"foo"));

	map_put(map,"bar",(void *)4);
	expect(__LINE__,50,(int)map_get(map,"bar"));

	map_put(map,"foo",(void *)6);
	expect(__LINE__,99,(int)map_get(map,"foo"));
	
}
