# 🔴 דוח תוצאות בדיקה סופי (Final Test Report)

**תאריך:** 2026-06-07  
**קובץ בדיקה:** comprehensive_test_balanced  
**סטטוס:** Segmentation Fault

---

## 📊 תוצאות

### ✅ **בדיקות שעברו בהצלחה: 20**

| קטגוריה | בדיקות | סטטוס |
|---------|--------|--------|
| Initialization | 4/4 | ✓ |
| Spawn Basic | 5/5 | ✓ |
| Spawn Errors | 3/3 | ✓ |
| Block/Resume | 5/5 | ✓ |
| Block/Resume Complex | 2/2 | ✓ |
| Sleep - Errors | 1/1 | ✓ |

### ❌ **בדיקה שנכשלה: 1**

```
[✓] Main thread cannot sleep
[✓] Spawned thread for sleep test
SEGMENTATION FAULT ← קורה כאן
```

---

## 🔍 **הבעיה בקיצור**

**Segmentation Fault** מתרחה כאשר main thread עושה context switch לthread 1 (child thread) בעת הביצוע של `uthread_sleep()`.

### **זרימת האירוע:**
1. main thread בדיקה עושה spawn של thread 1
2. main thread עושה `kill(getpid(), SIGVTALRM)` כדי לעשות context switch
3. timer_handler → schedule_next → context_switch(main → thread1)
4. thread1 מתחיל לרוץ מ-entry_point (sleeping_short_thread)
5. thread1 קורא ל-`uthread_sleep(1)`
6. uthread_sleep קורא ל-schedule_next
7. schedule_next עושה context_switch(thread1 → main)
8. **SEGMENTATION FAULT** בעת `siglongjmp(threads[0].env, 1)`

---

## 🎯 **ניתוח הגורם**

### **הבעיה:**
בקובץ `jump.c` בפונקציית `setup_jmpbuff()`:

```c
void setup_jmpbuff(sigjmp_buf *buff, char *stack, uint64_t stack_size, thread_entry_point entry_point)
{
    address_t sp = (address_t)stack + stack_size - sizeof(address_t);
    address_t pc = (address_t)entry_point;
    sigsetjmp(*buff, 1);  // ← זה שומר context של main, לא של thread החדש!
    (*buff)->__jmpbuf[JB_SP] = translate_address(sp);
    (*buff)->__jmpbuf[JB_PC] = translate_address(pc);
    sigemptyset(&(*buff)->__saved_mask);
}
```

### **מה קורה:**
1. כש-`sigsetjmp()` נקרא בתוך `setup_jmpbuff()`, הוא שומר את **context של main thread** בתוך `buff`
2. ואחרי זה עדכנים רק את Stack Pointer וProgram Counter
3. כאשר `siglongjmp()` חוזר לemain thread via `buff`, הוא משתמש בדברים שלא תואמים:
   - **Registers של main thread** (saved בsetp_jmpbuff)
   - **Stack של thread 1** (המשתנה SP)
   - → **Stack Corruption!**

---

## 💡 **הצעת התיקון**

### **הבעיה הטיפולית:**
Jump buffer של main thread לא בנוי כראוי מאחר שה-`sigsetjmp()` בתוך `setup_jmpbuff()` שמר context של main (לא של thread 1).

### **הפתרון:**

בתוך `uthread_init()`, נצטרך לשמור את context של main thread בצורה נכונה:

```c
int uthread_init(int quantum_usecs)
{
    // ...קוד קיים...
    
    // Initialize the main thread (tid 0)
    threads[0].tid = 0;
    threads[0].state = THREAD_RUNNING;
    threads[0].quantums = 1;
    threads[0].sleep_until = 0;
    threads[0].entry = NULL;
    is_blocked[0] = false;
    
    // ⭐ CRITICAL: Save main thread's context properly
    // This needs to be done BEFORE setting up the timer
    // because the timer will start generating signals immediately
    sigsetjmp(threads[0].env, 1);
    
    // Continue with signal handler and timer setup
    struct sigaction sa;
    sa.sa_handler = timer_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    
    if (sigaction(SIGVTALRM, &sa, NULL) == -1) {
        fprintf(stderr, "system error: sigaction failed\n");
        exit(1);
    }
    
    // Setup timer...
    // ...
}
```

### **למה זה יעבד:**
1. `sigsetjmp()` ישמור את context של main thread בתוך `threads[0].env`
2. כאשר thread אחר יעשה `siglongjmp(threads[0].env, 1)`, זה יחזור לmain עם הcontext הנכון
3. **Stack integrity** יישמר כיוון שכל thread יחזור לcontext שלו

---

## 📝 **סיכום**

| בעיה | סיבה | פתרון |
|------|------|--------|
| Segfault בsleep | main.env לא מאותחל נכון | sigsetjmp בתוך uthread_init |
| Context mismatch | setup_jmpbuff שמר context של main | Explicitly save main context |
| Stack corruption | ריגיסטרים לא מתאימים לstack | תקן את jump buffer |

---

## ✅ **צעדים הבאים**

1. **הוסף** `sigsetjmp()` בתוך `uthread_init()` לשמירת main context
2. **וודא** שהוא נקרא **לפני** `setitimer()`
3. **הרץ** את הטסטים שוב
4. כל בדיקות צריכות **להעביר בהצלחה**

---

**עדכון אחרון:** 2026-06-07 21:00  
**בעיה עיקרית:** Main thread context לא משומר כראוי  
**ממליץ:** Implement `sigsetjmp()` בתוך `uthread_init()`
