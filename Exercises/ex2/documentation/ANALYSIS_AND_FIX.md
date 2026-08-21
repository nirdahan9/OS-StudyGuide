📊 **סה"כ תוצאות** (**Final Analysis Report**)

## 🔴 **הבעיה:**
**Segmentation Fault** בעת context switch מmain thread לthread 1

### **מיקום הקריסה:**
```
[DEBUG] Before sigsetjmp for thread 0
[DEBUG] sigsetjmp returned 0, doing siglongjmp to thread 1
>>> Segmentation fault קורה כאן
```

---

## 🔍 **ניתוח הבעיה:**

### **הגורם:**
בקובץ `jump.c`, פונקציית `setup_jmpbuff()` עושה:
```c
void setup_jmpbuff(sigjmp_buf *buff, char *stack, uint64_t stack_size, thread_entry_point entry_point)
{
    // ... 
    sigsetjmp(*buff, 1);  // ← זה שומר context של main thread, לא של thread החדש!
    (*buff)->__jmpbuf[JB_SP] = translate_address(sp);
    (*buff)->__jmpbuf[JB_PC] = translate_address(pc);
    // ...
}
```

**זה הבעיה:**
- `sigsetjmp()` שומר את context של main thread (registers, stack pointer וכו')
- ואחר כך עדכנים רק SP וPC
- כשעושים `siglongjmp()` לthread 1, הוא משתמש ברגיסטרים של main עם stack חדש
- → **Stack corruption!**

---

## 💡 **הצעה לתיקון:**

### **אפשרות 1 (Recommended): Workaround בuthreads.c**

תסמוך על כך שkill() signal בתוך main לא יוצר context switch תוך כדי initialization:

**לפני:**
```c
int uthread_init(int quantum_usecs)
{
    // ...
    if (sigsetjmp(threads[0].env, 1) != 0) {
        return 0;
    }
    // ... setup ...
}
```

**אחרי:**
```c
int uthread_init(int quantum_usecs)
{
    // ... ללא sigsetjmp בתוך init
    threads[0].tid = 0;
    threads[0].state = THREAD_RUNNING;
    // ... setup ...
    return 0;
}
```

### **אפשרות 2: Modified context_switch**

בדוק אם thread הוא main thread וטיפול מיוחד:

```c
void context_switch(thread_t *current, thread_t *next)
{
    // ... קוד קיים ...
    
    if (current->tid == 0) {
        // main thread - special handling
        // עשה sigsetjmp ישירות לmain.env
        if (sigsetjmp(current->env, 1) == 0) {
            siglongjmp(next->env, 1);
        }
    } else {
        // regular thread
        if (sigsetjmp(current->env, 1) == 0) {
            siglongjmp(next->env, 1);
        }
    }
}
```

---

## 🎯 **המלצה הסופית:**

**הסיבה לבעיה:**
- `jump.c` עשה `sigsetjmp()` בתוך `setup_jmpbuff()`
- זה שומר context של main thread, לא context של thread החדש
- כש-`siglongjmp()` משתמש בcontext זה, זה גורם לstack corruption

**הפתרון:**
- הסר את `sigsetjmp()` מתוך `uthread_init()`
- שמור context של main thread בצורה שונה (או בכלל אל תשמור אותו בinit)
- תן לprogram להתחיל ולהתוך context switches כאשר צריך

---

## ✅ **הוצעה לתיקון מיד:**

1. **נוציא** את כל ה-debug statements מ-uthreads.c
2. **נוציא** את `sigsetjmp()` מתוך `uthread_init()`
3. **נחזור** לקוד המקורי פחות זה שינוי
4. **נריץ** את הטסטים שוב
