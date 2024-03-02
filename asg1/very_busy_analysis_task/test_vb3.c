/*
Name: Galangkangin Gotera
Matric number: A0274760Y
Email: galangkangin@u.nus.edu
*/
int main() {
   int a, b, c, x, y;


   // only b - a should be very busy because a + b is not in the third else
   if (a > b) {
     x = b - a;
     y = a + b;
   }
   else if(a < b) {
    x = a + b;
    y = b - a;
   } else {
    x = a + c;
    y = b - a;
   }
}

