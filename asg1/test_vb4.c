int main() {
   int a, b, x, y, w, z;
   
   // only a + b is very busy because b - a is not certain to get computed
   if (a > b) {
     x = b - a;
     y = a + b;
   }
   else if(a < b) {
    x = a + b;
    if(a * 2 > b) {
      y = b - a;
      return 0;
    }
   }
   
   y = w + z;
}

