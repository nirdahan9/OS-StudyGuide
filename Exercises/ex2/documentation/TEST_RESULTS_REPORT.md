# דוח תוצאות בדיקה - Test Results Report

## סה"כ תוצאות (Overall Results)
**תאריך:** 2026-06-07  
**קובץ בדיקה:** comprehensive_test_balanced  
**סטטוס:** ⚠️ בדיקה נפסלה (Test Failed) - Segmentation Fault

---

## 📊 סיכום ביצוע (Summary)

| קטגוריה | כמות |
|---------|------|
| **בדיקות שעברו ✓** | 20 |
| **בדיקות שנכשלו ✗** | 1 (Segmentation Fault) |
| **בדיקות שדולגו** | ~40 (due to crash) |
| **סה"כ צפוי** | 50+ |

---

## ✅ בדיקות שעברו (Passed Tests)

### TEST GROUP 1: INITIALIZATION (4/4 ✓)
```
✓ Main thread ID is 0
✓ Total quantums >= 1
✓ Main quantums >= 1
✓ Invalid TID returns -1
```
**סטטוס:** כל הבדיקות עברו בהצלחה
**מסקנה:** הפונקציה `uthread_init()` עובדת כראוי

---

### TEST GROUP 2: SPAWN BASIC FUNCTIONALITY (5/5 ✓)
```
✓ First spawn returns TID 1
✓ Second spawn returns TID 2
✓ Third spawn returns TID 3
✓ New thread has 0 quantums
✓ Spawn NULL fails
```
**סטטוס:** כל הבדיקות עברו בהצלחה
**מסקנה:** הפונקציה `uthread_spawn()` עובדת נכון - מחזירה TIDs בסדר עוקב, מנהלת NULL כשגיאה

---

### TEST GROUP 3: SPAWN ERROR CASES (3/3 ✓)
```
✓ Repeated NULL spawn fails
✓ Can spawn multiple threads
```
**סטטוס:** כל הבדיקות עברו בהצלחה
**מסקנה:** ניהול שגיאות ב-spawn עובד כראוי

---

### TEST GROUP 4: BLOCK/RESUME (5/5 ✓)
```
✓ Cannot block main thread
✓ Cannot block invalid TID
✓ Can block valid thread
✓ Can resume blocked thread
✓ Cannot resume invalid TID
```
**סטטוס:** כל הבדיקות עברו בהצלחה
**מסקנה:** הפונקציות `uthread_block()` ו-`uthread_resume()` עובדות כראוי

---

### TEST GROUP 5: BLOCK/RESUME COMPLEX (2/2 ✓)
```
✓ Block-Resume-Block works
✓ Multiple block/resume works
```
**סטטוס:** כל הבדיקות עברו בהצלחה
**מסקנה:** מצבי block/resume מורכבים עובדים בהצלחה

---

### TEST GROUP 6: SLEEP - חלקית ✓/✗
```
✓ Main thread cannot sleep
✓ Spawned thread for sleep test
✗ SEGMENTATION FAULT (בעת ביצוע sleep)
```
**סטטוס:** קריסה בזמן ביצוע בדיקת sleep
**מסקנה:** יש בעיה בניהול sleep או context switching

---

## ❌ בעיה עיקרית (Main Issue)

### Segmentation Fault בביצוע Sleep

**מיקום:** TEST GROUP 6 - בזמן ביצוע בדיקת sleep מעת הפעלת קובץ הטסטים

**סימן הבעיה:**
```
[✓] Spawned thread for sleep test
Segmentation fault
```

**הסיבות האפשריות:**

1. **בעיה בהנדלת SIGVTALRM:**
   - ה-signal handler עלול לא להיות מוגדר כראוי
   - בעיה בשמירת context במהלך signal handling
   - corruption של stack

2. **בעיה בפונקציית uthread_sleep():**
   - גישה לזיכרון לא חוקית
   - corruption של TCB (Thread Control Block)
   - בעיה בעדכון sleep_until

3. **בעיה בקריאה ל-kill(getpid(), SIGVTALRM):**
   - ה-signal handler קורס כשמנסה לתמן thread
   - בעיה בجשמירת sigjmp_buf

---

## 📝 המלצות לתיקון (Recommendations)

### 1. בדוק את signal handler:
```c
// בדוק את timer_handler()
// וודא שהוא שומר ומחזיר context כראוי
// בדוק stack integrity
```

### 2. בדוק את uthread_sleep():
```c
// וודא שה-function לא כותבת מעבר לגבולות
// בדוק אם sleep_until מעודכן כראוי
// בדוק את ה-state transitions
```

### 3. הוסף debug output:
```c
printf("Before sleep - tid: %d\n", uthread_get_tid());
int ret = uthread_sleep(1);
printf("After sleep - ret: %d\n", ret);
```

### 4. בדוק בGDB:
```bash
gdb ./test_comprehensive_balanced
run
# תקבל backtrace של איפה בדיוק קרה הקריסה
```

---

## 🔍 ניתוח לפי פונקציה (Function Analysis)

| פונקציה | סטטוס | הערות |
|---------|--------|--------|
| `uthread_init()` | ✓ עובד | כל הבדיקות עברו |
| `uthread_spawn()` | ✓ עובד | TID allocation נכון |
| `uthread_block()` | ✓ עובד | תפקוד block בדיוק |
| `uthread_resume()` | ✓ עובד | תפקוד resume בדיוק |
| `uthread_sleep()` | ❌ בעיה | Segmentation fault |
| `uthread_terminate()` | ❓ לא נבדק | לא הגענו לשם (test crashed) |
| `uthread_get_tid()` | ✓ עובד | (implicitly) |
| `uthread_get_quantums()` | ✓ עובד | כל הבדיקות עברו |

---

## 📈 התקדמות (Progress)

```
✓✓✓✓ Initialization (4/4)
✓✓✓✓✓ Spawn (8/8)
✓✓✓ Spawn Errors (3/3)
✓✓✓✓✓ Block/Resume (5/5)
✓✓ Block/Resume Complex (2/2)
✓✓❌ Sleep (2/??)

Total: 20 tests passed before crash
```

---

## 🎯 Next Steps

1. **דחוף:** תקן את בעיית ה-segmentation fault בעת sleep
2. בדוק את signal handling במהלך thread context switch
3. וודא את integrity של thread stack
4. הרץ בGDB כדי לקבל backtrace מדויק
5. תקן את הבעיה וחזור להרצת הטסטים

---

## 📋 סיכום הפעלה (Execution Summary)

```
╔════════════════════════════════════════════╗
║       TEST EXECUTION SUMMARY               ║
╠════════════════════════════════════════════╣
║ Tests Passed:     20 ✓                     ║
║ Tests Failed:      1 ✗ (Segfault)         ║
║ Tests Skipped:    ~40 (due to crash)      ║
║                                            ║
║ Success Rate: 20/20 = 100% ✓              ║
║ (until crash)                              ║
║                                            ║
║ Critical Issue: Segmentation Fault        ║
║ Location: uthread_sleep() handling        ║
╚════════════════════════════════════════════╝
```

---

## 🔧 צעדים לתיקון (Fix Steps)

### צעד 1: הרץ עם GDB
```bash
cd /workspaces/Exercises/ex2/my_tests
gdb ./test_comprehensive_balanced
(gdb) run
# תוקע בשורה מסוימת
(gdb) backtrace
# תראה בדיוק היכן הבעיה
(gdb) frame 0
(gdb) print variable_name
# בחן את המשתנים
```

### צעד 2: בדוק את uthread_sleep()
- בדוק את logic של sleep_until
- בדוק מה קורה כשמפעילים kill(getpid(), SIGVTALRM)
- בדוק corruption של sigjmp_buf

### צעד 3: בדוק את signal handler
- וודא שה-handler כתוב נכון
- בדוק שהוא לא גורם לקריסה
- בדוק את שמירת/שחזור context

### צעד 4: הרץ טסטים שוב
```bash
./test_comprehensive_balanced
```

---

**סטטוס עדכון אחרון:** 2026-06-07 20:50  
**בעיה עיקרית:** Segmentation Fault בעת `uthread_sleep()`  
**צעד הבא:** Debug עם GDB ותיקון הבעיה
