# הצעת תיקון (Fix Proposal)

## 🔴 הבעיה בקיצור

Main thread לא מאותחל עם `setup_jmpbuff()` או `sigsetjmp()`.

כשruntimeקורא `siglongjmp(threads[0].env, 1)` כדי לחזור לmain thread, 
זה קופץ לזיכרון לא מאותחל → **Segmentation Fault!**

---

## 💡 הפתרון

שמור את context של main thread בעת `uthread_init()` באמצעות `sigsetjmp()`.

### קוד התיקון:

```c
int uthread_init(int quantum_usecs)
{
    // Edge case: quantum_usecs must be positive
    if (quantum_usecs <= 0) {
        fprintf(stderr, "thread library error: quantum_usecs must be positive\n");
        return -1;
    }
    g_quantum_usecs = quantum_usecs;
    total_quantums = 1; 
    
    // Initialize all threads as unused
    for (int i = 0; i < MAX_THREAD_NUM; i++) {
        threads[i].state = THREAD_UNUSED;
        is_blocked[i] = false;
    }
    
    // Initialize READY queue (BEFORE initializing main thread)
    queue_init();
    
    current_tid = 0;
    
    // Initialize the main thread (tid 0)
    threads[0].tid = 0;
    threads[0].state = THREAD_RUNNING;
    threads[0].quantums = 1;
    threads[0].sleep_until = 0;
    threads[0].entry = NULL;
    is_blocked[0] = false;
    
    // ⭐ CRITICAL FIX: Save main thread's execution context
    // This allows context_switch() to properly return to main thread via siglongjmp
    // First call returns 0, allowing us to continue initialization
    // Subsequent calls from other threads return non-0, allowing them to exit gracefully
    if (sigsetjmp(threads[0].env, 1) != 0) {
        // We've been restored to this point from another thread
        // Just return to continue execution in main thread
        return 0;
    }
    
    // Set up the signal handler for SIGVTALRM
    struct sigaction sa;
    sa.sa_handler = timer_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    
    if (sigaction(SIGVTALRM, &sa, NULL) == -1) {
        fprintf(stderr, "system error: sigaction failed\n");
        exit(1);
    }
    
    // Set up timer
    struct itimerval timer;
    timer.it_value.tv_sec = g_quantum_usecs / 1000000;
    timer.it_value.tv_usec = g_quantum_usecs % 1000000;
    timer.it_interval.tv_sec = g_quantum_usecs / 1000000;
    timer.it_interval.tv_usec = g_quantum_usecs % 1000000;
    
    if (setitimer(ITIMER_VIRTUAL, &timer, NULL) == -1) {
        fprintf(stderr, "system error: setitimer failed\n");
        exit(1);
    }
    
    return 0;
}
```

---

## 🔍 איך זה עובד

### Initialization (בפעם הראשונה):
1. `sigsetjmp(threads[0].env, 1)` → חוזר **0**
2. ממשיך initialize signal handler וtimer
3. מחזיר 0 בהצלחה

### Context Switch (כשחוזרים לmain thread):
1. `siglongjmp(threads[0].env, 1)` → חזור לנקודת sigsetjmp עם return value **1**
2. `if (sigsetjmp(...) != 0)` → תנאי נכון, נכנסים לif
3. מחזיר 0 וממשיכים לexecute main thread

---

## 🎯 שינויים נדרשים

### בקובץ `uthreads.c`:

**שימו לב:**
- הוסיפו את ה-`sigsetjmp()` call בדיוק אחרי initialization של main thread
- העברו את `queue_init()` קודם (לפני שמשתמשים בקוד שקורא משהו)
- זה יפתור את הSegmentation Fault!

---

## ✅ צפוי להתקבל לאחר התיקון

```
[✓] Main thread ID is 0
[✓] Total quantums >= 1
[✓] Main quantums >= 1
[✓] Invalid TID returns -1

[✓] First spawn returns TID 1
[✓] Second spawn returns TID 2
... (יותר בדיקות יעברו)

[✓] Spawned thread for sleep test
[✓] Main thread cannot sleep
... (קריסה תפסיק, הטסטים יימשיכו!)
```

---

## ❓ שאלה - אתה בטוח בפתרון?

**כן**, כי:
1. Main thread צריך `sigjmp_buf` כמו כל thread אחר
2. `sigsetjmp()` בתחילת `init()` שומר את context של main
3. כשחוזרים עם `siglongjmp()`, return value הוא לא-0, ואנחנו יודעים שזה return

זה הפתרון הסטנדרטי לthreads שמשתמשים בsigsetjmp/siglongjmp!

---

## 📝 סיכום

| בעיה | סיבה | פתרון |
|------|------|--------|
| Segmentation Fault | main.env לא אתחל | sigsetjmp בתחילת init |
| Context not saved | אין jmp_buf לmain | sigsetjmp שומר context |
| Can't return | לא יודע לחזור | sigsetjmp return value |
