; ModuleID = 'builtin.c'
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

%struct._IO_FILE = type { i32, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, %struct._IO_marker*, %struct._IO_FILE*, i32, i32, i64, i16, i8, [1 x i8], i8*, i64, i8*, i8*, i8*, i8*, i64, i32, [20 x i8] }
%struct._IO_marker = type { %struct._IO_marker*, %struct._IO_FILE*, i32 }

@.str = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@stdin = external global %struct._IO_FILE*, align 8

; Function Attrs: nounwind uwtable
define void @print(i8* %str) #0 {
  %1 = alloca i8*, align 8
  store i8* %str, i8** %1, align 8
  %2 = load i8*, i8** %1, align 8
  %3 = call i32 @puts(i8* %2)
  ret void
}

declare i32 @puts(i8*) #1

; Function Attrs: nounwind uwtable
define i8* @prompt(i8* %prompt) #0 {
  %1 = alloca i8*, align 8
  %size = alloca i32, align 4
  %i = alloca i32, align 4
  %ret = alloca i8*, align 8
  %c = alloca i8, align 1
  store i8* %prompt, i8** %1, align 8
  store i32 8, i32* %size, align 4
  store i32 0, i32* %i, align 4
  %2 = load i32, i32* %size, align 4
  %3 = zext i32 %2 to i64
  %4 = call noalias i8* @malloc(i64 %3) #4
  store i8* %4, i8** %ret, align 8
  store i8 0, i8* %c, align 1
  %5 = load i8*, i8** %1, align 8
  %6 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str, i32 0, i32 0), i8* %5)
  br label %7

; <label>:7                                       ; preds = %0, %20
  %8 = load %struct._IO_FILE*, %struct._IO_FILE** @stdin, align 8
  %9 = call i32 @_IO_getc(%struct._IO_FILE* %8)
  %10 = trunc i32 %9 to i8
  store i8 %10, i8* %c, align 1
  %11 = load i8, i8* %c, align 1
  %12 = sext i8 %11 to i32
  %13 = icmp eq i32 %12, 10
  br i1 %13, label %18, label %14

; <label>:14                                      ; preds = %7
  %15 = load i8, i8* %c, align 1
  %16 = sext i8 %15 to i32
  %17 = icmp eq i32 %16, -1
  br i1 %17, label %18, label %20

; <label>:18                                      ; preds = %14, %7
  %19 = load i8*, i8** %ret, align 8
  ret i8* %19

; <label>:20                                      ; preds = %14
  %21 = load i8, i8* %c, align 1
  %22 = load i32, i32* %i, align 4
  %23 = add nsw i32 %22, 1
  store i32 %23, i32* %i, align 4
  %24 = sext i32 %22 to i64
  %25 = load i8*, i8** %ret, align 8
  %26 = getelementptr inbounds i8, i8* %25, i64 %24
  store i8 %21, i8* %26, align 1
  br label %7
}

; Function Attrs: nounwind
declare noalias i8* @malloc(i64) #2

declare i32 @printf(i8*, ...) #1

declare i32 @_IO_getc(%struct._IO_FILE*) #1

; Function Attrs: nounwind uwtable
define i32 @_Strlen(i8* %str) #0 {
  %1 = alloca i8*, align 8
  store i8* %str, i8** %1, align 8
  %2 = load i8*, i8** %1, align 8
  %3 = call i64 @strlen(i8* %2) #5
  %4 = trunc i64 %3 to i32
  ret i32 %4
}

; Function Attrs: nounwind readonly
declare i64 @strlen(i8*) #3

attributes #0 = { nounwind uwtable "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { nounwind "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { nounwind readonly "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #4 = { nounwind }
attributes #5 = { nounwind readonly }

!llvm.ident = !{!0}

!0 = !{!"clang version 3.8.0-2ubuntu4 (tags/RELEASE_380/final)"}
