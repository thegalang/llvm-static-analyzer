int main() {
   int a, b, x, y, w, z;

   if (a > b) {
     x = b - a;
     y = a + b;
   }
   else if(a < b) {
    x = a + b;
    y = b - a;
   } else {
    w = a + b;
    z = b - a;
    x = b * a;
   }
}

