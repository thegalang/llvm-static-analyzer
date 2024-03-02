int main() {
   int a, b, c, x, y;


   // c = y+b or c = x + b should not be included because x is redefined
   if (a > b) {
     x = b - a;
     y = a + b;
     c = y + b;
   }
   else  {
    x = a + b;
    y = b - a;
    c = x + b;
   }
}

