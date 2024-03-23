int main() {
    int x,y,z = 0;
    
    int i = 0;
    while(i < 100) {
        x = -((x + 2*y + 3*z) %3);
        y = (3*x + 2*y + z) % 11;
        z++;
    }
}