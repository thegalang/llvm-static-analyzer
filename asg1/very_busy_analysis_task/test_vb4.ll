; ModuleID = 'test_vb4.c'
source_filename = "test_vb4.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
entry:
  %retval = alloca i32, align 4
  %a = alloca i32, align 4
  %b = alloca i32, align 4
  %x = alloca i32, align 4
  %y = alloca i32, align 4
  %w = alloca i32, align 4
  %z = alloca i32, align 4
  store i32 0, i32* %retval, align 4
  %0 = load i32, i32* %a, align 4
  %1 = load i32, i32* %b, align 4
  %cmp = icmp sgt i32 %0, %1
  br i1 %cmp, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %2 = load i32, i32* %b, align 4
  %3 = load i32, i32* %a, align 4
  %sub = sub nsw i32 %2, %3
  store i32 %sub, i32* %x, align 4
  %4 = load i32, i32* %a, align 4
  %5 = load i32, i32* %b, align 4
  %add = add nsw i32 %4, %5
  store i32 %add, i32* %y, align 4
  br label %if.end12

if.else:                                          ; preds = %entry
  %6 = load i32, i32* %a, align 4
  %7 = load i32, i32* %b, align 4
  %cmp1 = icmp slt i32 %6, %7
  br i1 %cmp1, label %if.then2, label %if.else7

if.then2:                                         ; preds = %if.else
  %8 = load i32, i32* %a, align 4
  %9 = load i32, i32* %b, align 4
  %add3 = add nsw i32 %8, %9
  store i32 %add3, i32* %x, align 4
  %10 = load i32, i32* %a, align 4
  %mul = mul nsw i32 %10, 2
  %11 = load i32, i32* %b, align 4
  %cmp4 = icmp sgt i32 %mul, %11
  br i1 %cmp4, label %if.then5, label %if.end

if.then5:                                         ; preds = %if.then2
  %12 = load i32, i32* %b, align 4
  %13 = load i32, i32* %a, align 4
  %sub6 = sub nsw i32 %12, %13
  store i32 %sub6, i32* %y, align 4
  store i32 0, i32* %retval, align 4
  br label %return

if.end:                                           ; preds = %if.then2
  br label %if.end11

if.else7:                                         ; preds = %if.else
  %14 = load i32, i32* %a, align 4
  %15 = load i32, i32* %b, align 4
  %add8 = add nsw i32 %14, %15
  store i32 %add8, i32* %w, align 4
  %16 = load i32, i32* %b, align 4
  %17 = load i32, i32* %a, align 4
  %sub9 = sub nsw i32 %16, %17
  store i32 %sub9, i32* %z, align 4
  %18 = load i32, i32* %b, align 4
  %19 = load i32, i32* %a, align 4
  %mul10 = mul nsw i32 %18, %19
  store i32 %mul10, i32* %x, align 4
  br label %if.end11

if.end11:                                         ; preds = %if.else7, %if.end
  br label %if.end12

if.end12:                                         ; preds = %if.end11, %if.then
  %20 = load i32, i32* %w, align 4
  %21 = load i32, i32* %z, align 4
  %add13 = add nsw i32 %20, %21
  store i32 %add13, i32* %y, align 4
  br label %return

return:                                           ; preds = %if.end12, %if.then5
  %22 = load i32, i32* %retval, align 4
  ret i32 %22
}

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 10.0.0-4ubuntu1 "}
