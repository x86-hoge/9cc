
int main(){
    int a;
    a=hikaku(0,0);
    
    a=hikaku(42,42);
    
    a=hikaku(21,5+20-4*1);
    
    a=hikaku(41,12 + 34 - 5);
    
    a=hikaku(47,5+6*7);
    
    a=hikaku(15,5*(9-6));
    
    a=hikaku(4,(3+5)/2);
    echo(0);
    0;
}

int hikaku(int a,int b){
    if(a==b){

        return 0;
    }
    outerr("データが一致しませんでした。",a,b);
    return 1;
}