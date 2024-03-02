/*
Name: Galangkangin Gotera
Matric number: A0274760Y
Email: galangkangin@u.nus.edu
*/
int main() {
   int a, b, c, d, x, y;


   // y + b or x + b should not be included because x and y is redefined
   if (a > b) {
     x = b - a;
     y = a + b;
     c = y + b;
     d = x + b;
   }
   else  {
    x = a + b;
    y = b - a;
    c = x + b;
    d = y + b;
   }
}

