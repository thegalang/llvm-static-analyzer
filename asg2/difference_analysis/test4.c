int main() {
    int a, x = 10,z=0;
    
    int i = 0;
    while(i++ < 100) {
        
        if(a * 2 == 0) {
            x = x * 2;
        }

        if(a * 3 == 0) {
            x = x / 3;
        }

        if(a * 4== 0) {
            x = x + 5;
        }

        if(a * 5 == 0) {
            x = x - 10;
        }

        // sep(x, z) should be infinity because it will keep taking x*2
    }
}