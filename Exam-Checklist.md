# ✅ צ׳קליסט למידה למבחן — Operating Systems

> **קורס:** Operating Systems, אוניברסיטת רייכמן, סמסטר אביב 2026. מרצה: Tsvi Cherny-Shahar · מתרגלים: Daniel Karalnik, Liam Tal.
> **מקורות שנסרקו:** כל 7 קבצי ההרצאות (`Lectures-md/`), כל 7 קבצי התרגולים (`Recitations-md/`), 3 מערכי פתרונות תרגילי בית + מפרט תרגיל 1 (`Solutions-md/`), מבחן התרגול עם פתרונות מלאים (`Exams-md/`), וקובץ הסיכום המרכזי `MASTER.md`.
> **מבנה המבחן** (מ-`Practice-Exam`): 3 שעות, 100 נק׳ — **חלק א׳:** 10 שאלות נכון/לא-נכון × 3 נק׳ = 30 (עונים על הכול); **חלק ב׳:** 4 שאלות אמריקאיות × 5 נק׳ = 20 (עונים על הכול); **חלק ג׳:** 3 שאלות פתוחות × 25 נק׳, **עונים על 2 מתוך 3** = 50. ניקוד לפי תת-סעיף — **חובה להראות עבודה**.
> **שקלול הקורס:** 3 תרגילים = 40% (13.3% כל אחד), מבחן = 60% (חובה ≥60 כדי לעבור).
> **⚠️ כל יחידות האחסון ב-base-2:** 1KB = 1024B, 1MB = 1024KB, 1GB = 1024MB (כלומר KiB/MiB/GiB) — המבחן קובע זאת מפורשות.
>
> **איך משתמשים:** עוברים נושא-נושא ומסמנים `[x]` כשאתם יכולים לפתור תרגיל בנושא ו/או להסביר אותו **בלי** להסתכל בחומר. סעיף שאתם לא בטוחים בו — סמנו והחזירו אליו.
> **הערת היקף:** כל נושא ברשימה נלמד בקורס ונמצא בחומר הבחינה, **פרט** ל-**וירטואליזציה (יחידה 21)** שלפי הערת המתרגל בהקלטה היא נושא בונוס שכנראה **לא במבחן** — היא מסומנת 🔻 ומרוכזת גם בסוף תחת "מחוץ לחומר". אשרו מול צוות הקורס לפני שמוותרים עליה לגמרי.

---

## ⚠️ אזהרת אסימטריה — אל תלמדו רק מסלול אחד

מסלול ההרצאות ומסלול התרגולים רצים במקביל אך **אינם** כפילויות. נושאים שקיימים רק במסלול אחד (מאומת):

| נושא | קיים רק ב- | הערה |
|---|---|---|
| **Executables, ELF, linking, PLT/GOT, COW, PIC** | הרצאה 7 (יח׳ 20) | אין תרגול שמכסה זאת — **מקור יחיד**. |
| **FIFO, Belady's Anomaly, thrashing, EAT, demand paging** | תרגול 5 (יח׳ 14) | לא מופיע כלל בהרצאה 5 — חומר מבחן קלאסי. |
| **BIOS, UEFI, MBR, GPT, תהליך ה-boot, NTFS journaling, FDs after `fork()`** | תרגול 6 (יח׳ 17–18) | לא מופיע בהרצאה 6. |
| **Peterson, ticketlock (TAS/FAA), memory barriers לעומק** | הרצאה 2 (יח׳ 4–5) | לא מופיע בתרגול 2. |
| **`fork()` לעומק: zombies, `exit()` מול `pthread_exit()`, fork-in-thread** | תרגולים 2 ו-6 | טופל עמוק יותר מבהרצאות. |
| **וירטואליזציה, hypervisors, containers** | תרגול 7 (יח׳ 21) | אין הרצאה — 🔻 כנראה לא במבחן (ראו למטה). |

---

## 🧰 חלק א׳ — יסודות: מהי מערכת הפעלה

### יחידה 1 — היסטוריה ותפקיד מערכת ההפעלה  ·  *(OS1 + Rec1)*
- [ ] מחשבים ראשונים — **The Bombe**: התקן אלקטרו-מגנטי (**electro-magnetic device**) שתכנן Alan Turing ושיכלל Gordon Welchman (1940) לפיצוח ה-**Enigma**; חומרה קשיחה שמריצה **תוכנית יחידה** בלבד
- [ ] **ENIAC** (דור 1, 1945-1955) — המחשב ה-**general purpose Turing-complete** הראשון, ולכן ניתן לתכנות מחדש; תוכנת באמצעות **plug-boards** ובמחשבי דור 1 מאוחרים גם **vacuum tubes**; ששת המתכנתות הראשונות
- [ ] דור 2 — מחשבי טרנזיסטורים (1955-1965): תכנות ב-**punch cards** ו-**magnetic tape**, היווצרות ספריות (libraries) של כרטיסים לשימוש חוזר, אך עדיין הפעלה **ידנית** של המחשב ע"י המתכנת
- [ ] **Batch processing** — אוסף כרטיסים (batch) = תוכנית; צריבת כרטיסים לסרט מגנטי לשיפור ביצועים; שרשרת המכונות IBM 1401 → 7151 → 1403 (כרטיס → סרט → הדפסה); ה"בזבוז" של configuration &amp; setup של ה-plug-boards ותור פיזי של המתכנתים
- [ ] **Robert L. Patrick** ב-GM (1954) — "סחיטת" עוד ריצות ע"י תוכנית מוכנה-מראש על סרט + **change deck**; ארבע התובנות/התרומות שלו (הורדת מתכנתים מהקונסולה, תקנון setup, מניעת reload, שימוש חוזר בקוד)
- [ ] **GM I/O (GMR Monitor)** — מערכת ההפעלה הראשונה; שלושה שלבים: batch → run → convert output &amp; print; במהותה תוכנית שתפקידה **להריץ תוכניות אחרות**
- [ ] דור 3 — **Multi-Programming &amp; Time-Sharing** (1965-1980): כש-program ממתין ל-peripheral (מדפסת) ה-CPU יכול להריץ jobs אחרים; **SPOOL**; שימוש ב-**job pool** ו-**scheduling** לבחירת ה-job הטוב לשיפור **CPU Utilization**; **time-sharing** = החלפה לפי חלון-זמן
- [ ] דורות מאוחרים — דור 4 (מ-1980): מחשב אישי ומכשירי כף-יד; דור 5: **multi-core**, **virtualization**, **containerizing**, **subsystems**
- [ ] מבנה ה-PC — ה-**bus** המשותף מחבר את כל רכיבי החומרה (CPU, memory, controllers והמכשירים); **Interrupt** (חומרה): רכיב מסמן ל-CPU דרך ה-bus, ה-CPU קופץ (**JMP**) לפי ה-**interrupt table** ע"י קביעת ה-**Instruction Pointer** (Program Counter), והערך הקודם נשמר ומשוחזר
- [ ] תהליך האתחול (boot) — ה-**IP** מקובע ל-**0xFFFFFFF0**; קוד ה-**UEFI** (או **BIOS** ישן) כ-firmware על ה-motherboard; **POST** (כולל ביפ מה-PC-speaker); איתור **bootable device** לפי הגדרות ב-**NVRAM/CMOS** וסדר האתחול
- [ ] ה-**Bootloader הוא ה-OS** — ה-UEFI מוצא **partition** מסוג **EFI System Partition (ESP)** בפורמט **FAT32**, ומאתר ומריץ את קובץ ה-bootloader; ה-**.efi** בפורמט **PE (Portable Executable)** — אותו פורמט כמו הרצות Windows (ה-UEFI חייב להבין partitions, FAT ו-PE)
- [ ] הפשטות מערכת ההפעלה (**OS abstractions**): **Thread** (הרצת ה-CPU), **Address Space** (זיכרון), **File** (מבנה האחסון הפנימי), **Process** (קושר יחד את כל ישויות התוכנית הרצה); מטרת ההפשטות — הסתרת מורכבות החומרה וממשק אחיד
- [ ] ה-**thread** של ה-**process** — ב-**MS-DOS** thread יחיד מריץ גם את ה-OS וגם אפליקציות; ב-OS מודרני (Windows/macOS/Android) נוצר **main thread** לכל process, והתוכנית רצה כל עוד ה-main thread רץ; ריבוי תוכניות concurrently ⇐ ריבוי threads concurrently
- [ ] **Concurrency vs. Parallelism** (מושג) — **concurrency**: משימות בפרקי-זמן **חופפים** (interleaved, core יחיד); **parallelism**: משימות **בו-זמנית** (דורש כמה cores); *"dealing with" מול "doing" מספר דברים בו-זמנית*
- [ ] **Context Switch** ו-**Scheduling** — כמה processes על core יחיד "לוקחים תורות" וה-CPU מחליף context (גם בין threads של אותו process); זו פעולה **"כבדה"**; ה-OS מחליט (**schedules**) איזה thread ירוץ הבא, וה-**priorities** של processes/threads משפיעות על ה-scheduler
- [ ] **Process Address Space** — 5 מקטעים: **Code/Text**, **Data** (סטטי), **Heap** (malloc/new, גדל מטה), **Stack** (משתנים מקומיים + call stack, גדל מעלה), **OS Metadata**; שיתוף זיכרון בין threads — **משותף**: Code, Heap, Global variables, File handles; **לא משותף**: Stack, Registers, Program counter

### יחידה 2 — טרמינולוגיה וחישובי זמן  ·  *(OS1 + Rec1 + ex1)*  📐 *נומרי, נפוץ במבחן*
- [ ] הגדרות ליבה: **Latency** (הזמן להשלמת job, יחידת זמן) ו-**Throughput** (כמות נתונים ליחידת זמן, למשל GB/s)
- [ ] **CPU Utilization** — הזמן שבו ה-CPU מריץ קוד **שאינו OS** (non-OS code)
- [ ] **Overhead** — זמן שמושקע במשימות שאינן קוד התוכנית (גישה לזיכרון, חישובי OS, וגם context switch)
- [ ] **CPU Usage = Utilization + Overhead** — שים לב: **Utilization ≠ Usage**
- [ ] **CPU Idle Time = Total − Usage** — ה-Usage לא חייב להיות 100%; ה-CPU idle בהמתנה ל-I/O (דיסק/רשת), לקלט משתמש, לאירועים/סיגנלים, או כשאין task ב-scheduler queue
- [ ] **Time Accounting** — ארבע מדידות זמן: **Wall Time** (זמן קיר/סטופר), **User Time** (קוד ב-**user mode**), **System Time** (קוד **kernel** עבור התוכנית — scheduling, context switch), **Wait Time** (ה-thread חי אך **לא רץ**: preempted/blocked/sleeping)
- [ ] נוסחאות: **CPU Time = User + System**; **Wait Time (per thread) = Wall − Execution**; **Total Wait = Σ** (per thread); single-core: User+System ≤ Wall, multi-core: **עלול לחרוג**
- [ ] **Wait Time** — single-thread (I/O, sleep, קלט משתמש) מול multi-threaded (**preemption**, המתנה ל-core, threads אחרים רצים); ב-multi-thread ה-total wait **גדל** כי ה-threads חולקים CPU (גם ללא I/O)
- [ ] **תרגול (Ex1):** חישוב utilization/overhead/usage/throughput/latency מתרחיש 10s (6s קוד, 2s זיכרון, 2s OS, 500GB) → 6s / 4s / 10s / **50 GB/s** / 10s; פירוק אחוזים 60% קוד מול 40% overhead
- [ ] **תרגול (Ex1b — Idle CPU):** תרחיש 12s (5s קוד, 2s OS, 1s זיכרון, שאר idle, 360GB) → util 5s, overhead 3s, usage 8s, **idle = 12−8 = 4s ≈ 33.3%**, throughput 30 GB/s, latency 12s; פירוק אחוזים 41.7% + 25% + 33.3% = 100%
- [ ] **תרגול (Ex2 — Context Switching):** 3 processes ב-15s (exec 4+3+5=12s; CS 1s + זיכרון 1s + OS 1s, 600GB) → util 12s, overhead 3s, usage 15s, throughput 40 GB/s, **latency = Total/Jobs = 15/3 = 5s/job**
- [ ] **תרגול (Ex3 Part A — ללא overhead):** 5 threads × 40ms (200ms עבודה); חישוב wall/user/system/wait והכרעה concurrency/parallelism/both — single-core: **200 / 200 / 0 / 800**ms (concurrency ללא parallelism), 4-core: **⌈5/4⌉×40 = 80 / 200 / 0 / 40**ms (concurrency עם parallelism, User&gt;Wall) *(גזירה מלאה + טבלת per-thread)*
- [ ] **תרגול (Ex3 Part B — Context Switching):** CS כל 5ms בעלות 0.2ms (כולל אחרי ה-chunk האחרון) → 8 chunks/thread, 40 CS, **system = 8ms**, round = 26ms, CPU burst/thread = 41.6ms, סיום מדורג ב-5.2ms, **wall = 208ms**, **total wait = 780ms** (נמוך מ-800ms הסדרתי) *(גזירה מלאה)*
- [ ] **תרגול (TQ1 — פתרון ex1):** 3 threads × 40ms ללא I/O — single core = **120ms** (concurrency בלבד), 3-core = **40ms** (parallelism מושלם), overhead נטו מ-context switches = 24 × 0.2 = **4.8ms** *(כולל גזירה)*

---

## 🧵 חלק ב׳ — Threads, מקביליות וסנכרון  *(היחידה הגדולה בקורס)*

### יחידה 3 — אנטומיה של תהליך ו-Thread  ·  *(OS2 + Rec2 + ex1)*
- [ ] ה-**Stack** הוא בלוק זיכרון; כתובת ראש המחסנית נשמרת ב-**Stack Pointer (SP)** — **ESP**/**RSP** (32/64 ביט), וכתובת הבסיס ב-**Base Pointer (BP)** — **EBP**/**RBP**; **RIP** הוא ה-Instruction Pointer
- [ ] משתנים לוקאליים נשמרים על ה-Stack ומשוחררים אוטומטית; מבנה **stack frame** עם padding ליישור; `alloca()` מקצה על המחסנית; הקצאה/שחרור על המחסנית זולים מאוד
- [ ] פקודות אסמבלי לניהול המחסנית: **PUSH**, **POP**, **JMP**, **CALL**, **RET** והמימוש שלהן דרך RSP/RIP
- [ ] **Prologue &amp; Epilogue** של פונקציה (`push rbp` / `mov rbp,rsp` … `pop rbp` / `ret`)
- [ ] **Call stack** — קריאות מקוננות (k→h→g) ותצוגת ה-call stack בדיבאגר
- [ ] מבנה זיכרון של תהליך — 5 מקטעים: **Code**, **Data**, **Heap**, **Stack**, **Metadata**; ה-Heap גדל כלפי מעלה וה-Stack כלפי מטה
- [ ] **Thread stack** — לכל thread מחסנית משלו, פועלת **LIFO**, מאחסנת משתנים לוקאליים/stack frames/כתובות חזרה; ה-OS מקצה לכל thread מחסנית נפרדת (מתוך ה-Heap); גודל מחסנית ברירת-מחדל ניתן לשינוי; חריגה גורמת ל-**stack overflow**
- [ ] **PCB** (Process Control Block) ושדותיו (PID, State, Priority, מצביעים ל-Code/Data/Heap, Threads, User, Environment (עותק), Open Files, TLS, Exit Code)
- [ ] **TCB** (Thread Control Block) ושדותיו (Thread ID, **Registers/context**, Parent PID, dynamic priority, מצביע ל-TLS); PCB אחד לתהליך, מספר TCB-ים (אחד לכל thread)
- [ ] **Context switch** — שמירת `TCB_i.context` וטעינת `TCB_j.context`; ה-context כולל SP/PC/registers; מהווה overhead (אף עבודה לא רצה בזמן ההחלפה)
- [ ] **Thread States** — New, Ready, Running, Waiting/Wait, Terminated והמעברים (admit, scheduler picks, preempt/switch, exit, wait event, event signaled)
- [ ] סיום ה-main thread: ב-**Windows** התהליך מסתיים, ב-**Linux** לא; כשתהליך מסתיים ה-OS מנקה משאבים; אם תהליך אחר "משתמש" בו הוא נכנס למצב **zombie**
- [ ] **Scheduler &amp; Dispatcher** — ה-CPU Scheduler בוחר מי ירוץ ולכמה זמן; ה-Dispatcher מבצע context switch ומעבר בין user mode ל-kernel mode
- [ ] יצירת תהליך — Windows `CreateProcess` (מריץ executable חדש) מול Linux **fork** (יוצר **עותק**) + `execl()` (מחליף בתהליך חדש לגמרי)
- [ ] **fork** מעתיק זיכרון, file handles ואת ה-thread הקורא (ולכן RIP זהה); **fork bomb**; ערכי החזרה (הורה מקבל PID של הילד, הילד מקבל 0, שגיאה -1)
- [ ] מה `fork()` מעתיק (address space, open FDs, signal handlers, env vars, cwd) ומה לא (PID, PPID, pending signals, שאר ה-threads — רק ה-thread הקורא); **Copy-on-Write (COW)**
- [ ] משפחת `exec()` — מחליפה קוד/זיכרון, אותו PID, לא חוזרת בהצלחה; וריאנטים (`execl`/`execv`/`execlp`/`execvp`); דפוס **fork()+exec()** + `wait()`/`waitpid()` ל-reap
- [ ] יצירת thread — Windows `CreateThread` + `WaitForSingleObject`; Linux `pthread_create` + `pthread_join` (הקצאת stack, אתחול TCB, entry point)
- [ ] **join** מול **detach** — joinable (ברירת מחדל, מחזיק משאבים עד `pthread_join`, מחזיר ערך) מול detached ("fire and forget", משאבים משוחררים אוטומטית)
- [ ] סיום thread מול תהליך — `exit()`/return מ-main מסיימים את כל התהליך, מול `pthread_exit()`/`pthread_cancel()`/return מפונקציית ה-thread שמסיימים thread בודד
- [ ] **zombie** (defunct) — מנגנון (termination → retention בטבלת התהליכים → reaping ע"י `wait()`); סכנת **PID exhaustion**; נראה ב-`ps` כ-Z/`&lt;defunct&gt;`
- [ ] **Orphan** מול **zombie** — orphan הוא תהליך חי שההורה שלו מת (מאומץ ע"י **init**/PID 1) מול zombie שהוא תהליך מת שממתין ל-reaping; init מאמץ ומנקה יתומים
- [ ] מוטיבציה ל-multi-threading — thread שמצייר חלון מול thread ששומר לדיסק (רספונסיביות / "Not Responding")
- [ ] **IPC** — intra-process מול inter-process; **Shared Memory Model** מול **Message Passing Model** (המודל היחיד ברשתות מחשבים); שיתוף זיכרון בין תהליכים דרך **memory-mapped files**; כל ה-threads של תהליך P חולקים את הזיכרון של P
- [ ] **תרגול:** `exit()` מול `pthread_exit()` — אילו פלטים אפשריים ומדוע `B2`/`Done` לעולם לא מודפסים
- [ ] **תרגול:** `fork()` בתוך thread — כל סדרי הפלט האפשריים; לילד יש רק את ה-thread שקרא ל-fork
- [ ] **תרגול:** מספר threads + `fork()` — האם לילד יש את `t1`; אילו פלטים אפשריים
- [ ] **תרגול:** חישוב מקבילי בסכומים חלקיים (`partial_sum` לכל thread, סכום 0-99=4950)
- [ ] **תרגול:** TQ1 — concurrency מול parallelism: זמן קיר על ליבה בודדת (120ms), על 3 ליבות (40ms), ותוספת ה-overhead של context switches (4.8ms)

### יחידה 4 — Race Conditions ומניעה הדדית  ·  *(OS2 + Rec2 + ex1)*
- [ ] **Atomic operation** — פעולה שלוקחת מחזור CPU אחד; פעולה לא-אטומית עלולה "להתפצל" ע"י context switch; `x++` מתקמפל ל-3 פקודות (`LW`/`ADD`/`SW`); לא כל opcode אטומי (PUSH/POP/CALL); ב-Rec2: פעולה אטומית = צעד יחיד בחומרה, בסיס למנעולים; השוואת pseudo-assembly לא-אטומי מול `ATOMIC_ADD`
- [ ] **Threads Interleaving** — ללא סנכרון threads רצים בכל סדר, מתחילים/עוצרים בכל זמן, לכל משך; (מתקדם) crash ו-reorder של פקודות; אי-דטרמיניזם בתזמון
- [ ] חשיבות הסנכרון — **race conditions**, **deadlocks**, **starvation**; סקירת מנגנוני סנכרון (mutex, semaphores, locks, barriers, atomics)
- [ ] **Mutual Exclusion** — Dijkstra (1965); הגדרת מניעה הדדית והגדרת **critical section**
- [ ] מבנה ה-**Critical Section** ב-4 אזורים: remainder → entry → critical → exit (בלולאה)
- [ ] תכונות נדרשות: **Mutual Exclusion**, **Deadlock-freedom**, **Starvation-freedom**, **Livelock-freedom**; זיהוי ה-critical section (`x++`)
- [ ] **Busy Wait** / **Spinlock** / **Wait(Block)** — הגדרות וטבלת trade-off (בזבוז CPU מול context switch)
- [ ] **Peterson's algorithm** (1981) — `flag[]` + `turn`; ניתן להכללה ל-n threads; **(כולל הוכחה)** של מניעה הדדית
- [ ] **תרגול:** `multithread.c` — שני threads שמגדילים משתנה גלובלי מיליון פעם ("מה הפלט?")
- [ ] **תרגול:** trace צעד-אחר-צעד של `i→5 ; i++` — interleaving של שני threads, בעיית **lost update** (X יורד מ-4 ל-1; בריצה השנייה X=2)
- [ ] **תרגול:** tryout 1 (משתנה `turn`) — **(כולל הוכחה)** של מניעה הדדית, וכישלון בהיבט ה-starvation
- [ ] **תרגול:** tryout 2 (`flag` — קביעה ואז בדיקה) — trace שמוביל ל-**deadlock/livelock**
- [ ] **תרגול:** tryout 3 (`flag` — בדיקה ואז קביעה) — trace שמראה **אין מניעה הדדית**
- [ ] **תרגול:** bank account interleaving — הפקדה/משיכה, יתרות אפשריות $50/$150/$200, **lost update** (Rec2)
- [ ] **תרגול:** TQ2 — interleavings של `x` (ערכים סופיים אפשריים 1 או 2) עם ריצה מפורשת לכל ערך

### יחידה 5 — מנעולים ופרימיטיבים לסנכרון  ·  *(OS2 + Rec2 + HW1)*  🔑 *היחידה הגדולה והמרכזית*
- [ ] **Read-Modify-Write (RMW)** — אופרטורים אטומיים בחומרה; מדוע לעיתים מעדיפים CPU פשוט בלעדיהם (צריכת חשמל, ריבוי ליבות, embedded/עלות)
- [ ] שלושת אופרטורי ה-RMW: **TAS** (test-and-set), **FAA** (fetch-and-add), **CAS** (compare-and-swap) — pseudocode
- [ ] **Atomic Increment** — מימוש עם FAA; ואם יש רק CAS — לולאת busy-wait עם CAS
- [ ] ספריות/טיפוסים ב-C: `stdatomic.h`, `sched.h`, `atomic_flag`, מילת המפתח `volatile` (לא ממקם memory barrier)
- [ ] API של `stdatomic.h` (Rec2) — `atomic_load`/`store`/`fetch_add`/`fetch_sub`/`exchange`/`compare_exchange_strong`/`flag_test_and_set`/`flag_clear`; `atomic_flag` = boolean lock-free
- [ ] **Spinlock (TAS)** — `init`/`acquire`/`release` עם `sched_yield()`; תכונות: מניעה הדדית כן, deadlock-free כן, **starvation-free לא** **(כולל הוכחה)** — thread אחד יכול להיכנס שוב ושוב בעוד אחר "לא מצליח"; queue פותר
- [ ] **Alan Burns** TAS starvation-free — מערך `waiting[]` + `key`, חלוקה לשלבים (Announce / Try / CS / Find next / release)
- [ ] **Ticketlock** — FIFO spinlock עם FAA: `ticket`/`cur_ticket`, `acquire`/`release`; אנלוגיית מכונת המספרים בדלפק
- [ ] **FIFO Ticketlock ללא busy-wait** — suspend + interrupt במקום polling; Windows `SuspendThread`/`ResumeThread`, Linux `sigwait`/`pthread_kill`; מבנה `tlint`
- [ ] **Memory barrier / fence** — out-of-order execution ו-cache מבוזר ב-multi-core; `mm_lfence`/`mm_sfence`/`mm_mfence`; `volatile` ב-C/C++ לא ממקם barrier אבל ב-Java/C# כן (full barrier)
- [ ] בעיית האופטימיזציה (Rec2) — אופטימיזציות קומפיילר (caching ברגיסטרים) ו-CPU (reordering, out-of-order); `volatile` מונע caching אבל **לא** מבטיח אטומיות ולא מונע reordering
- [ ] **Memory fences** למניעת reordering — `atomic_thread_fence` עם סמנטיקת **release** (כותב) / **acquire** (קורא) (Rec2)
- [ ] טקסונומיה של **synchronization primitives** — נמוכי-רמה (atomics, spinlock, memory barriers) מול גבוהי-רמה (semaphore/mutex, condition variables, readers/writers)
- [ ] **Semaphore** — Dijkstra (1963); `P` (proberen/wait) ו-`V` (verhogen/signal); אתחול במספר משאבים; "1" = **mutex**; **binary** מול **counting**; **bounded** מול **unbounded**; מנגנון (count + תור המתנה) ו-API
- [ ] **Thread barrier** — עוצר N threads עד שכולם מגיעים לנקודה; `init`/`wait`; trace אנימציה ל-3 threads
- [ ] **Readers-Writer lock** — משאב משותף M; readers במקביל, writers בבלעדיות; טבלת מצבים (readers/writer); מדיניות תור: **reader-priority**/**writer-priority**/**fair (FIFO)**; API; שימושים (DB, file systems, caches); גרסת **Upgradable** — upgrade/downgrade אטומיים
- [ ] **Condition Variable** — במקום polling; `wait()` שנרדם ומתעורר; **signal** (מעיר אחד) מול **broadcast** (מעיר את כולם); משמש עם mutex; לולאת `while` בגלל **spurious wakeups**; API עם `ext_lock`
- [ ] **[HW1]** PQ1 — **unbounded semaphore** מעל **ticket lock**: `wait` (P) עושה busy-wait עם `sched_yield()` כש-value=0, `signal` (V) מגדיל; ה-value מוגן ע"י ה-ticket lock; FIFO fairness לא נדרש
- [ ] **[HW1]** PQ2 — **condition variable** מעל `ticket_lock` חיצוני: רישום ה-thread כממתין *לפני* שחרור הנעילה (בלי lost notifications), שחרור ורכישה מחדש של `ext_lock`; `signal` מעיר אחד, `broadcast` מעיר את כולם; signals לא נזכרים אם אין ממתינים
- [ ] **[HW1]** PQ3 — **writer-preference readers-writer lock**: מניעת starvation של writer (אם writer ממתין — קוראים חדשים נחסמים); מונים `active_readers`/`waiting_writers`/`active_writer`; קורא נכנס רק אם `active_writer==0 &amp;&amp; waiting_writers==0`
- [ ] **תרגול:** bank account — safe withdrawal: באג check-then-act (יתרה שלילית), תיקון בלולאת **CAS** (Rec2)
- [ ] **תרגול:** connection pool (N=3) — mutex מגביל מדי, unbounded semaphore חורג מהמגבלה ב-`post` נוסף, bounded semaphore פותר (Rec2)
- [ ] **תרגול:** writer starvation — reader-priority מרעיב writer; פתרונות FIFO/backoff; writer-priority רק מזיז את הבעיה; timeout אינו פתרון (Rec2)
- [ ] **תרגול:** lock upgrade deadlock — `rdlock`→`wrlock` נכנס ל-deadlock; תיקון release+re-acquire (עם re-check); חלופת mutex; trade-offs של mutex מול rwlock (Rec2)
- [ ] **תרגול:** two conditions, one CV — deadlock כש-`signal` מעיר את ה-waiter הלא-נכון; פתרון broadcast או CV נפרד לכל תנאי; ו-resource pool with cancellation (Rec2)
- [ ] **תרגול:** אילוסטרציה של semaphore — שלוש עמדות עבודה ודלפק קבלה, מעקב אחר ה-counter צעד-אחר-צעד (OS2)
- [ ] **תרגול:** TQ3 — TAS מול ticket lock: **(כולל הוכחה)** מי מבטיח סדר FIFO, מי מבטיח bounded waiting (חסם k−c), ומדוע TAS יכול להרעיב thread

### יחידה 6 — בעיית היצרן-צרכן (Producer-Consumer)  ·  *(OS2 + Rec2 + ex1)*
- [ ] **Producer-Consumer** — בעיית סנכרון קלאסית עם **bounded buffer**; אתגרים (buffer מלא → producer ממתין, ריק → consumer ממתין, race על גישה); פתרון: mutex + CV `not_full` + CV `not_empty`
- [ ] Walkthrough צעד-אחר-צעד (Rec2) — 4 שלבים: מצב התחלתי (ריק), producer מוסיף ומאותת, consumer מתעורר וצורך, buffer מלא (producer ממתין)
- [ ] **תרגול:** מימוש ב-C (Rec2) — שני condition variables, לולאות `while` לתנאים, producer מאותת ל-consumer ולהפך
- [ ] ניתוח נכונות (Rec2) — **race-free** (הכל מוגן ב-mutex), **deadlock-free** (mutex יחיד, אין circular wait, `wait` משחרר את ה-mutex), **starvation-free** (CV נפרדים לכל צד)
- [ ] תרחיש ה-consumers-producers ב-OS2 — שלוש עמודות ו-queue של 7 תאים; מחזור מלא (consumers ישנים על ריק → producers ממלאים → צריכה בעוד producers ישנים על מלא); condition variables בשני הצדדים (`is_full` ליצרנים, `is_work` לצרכנים) והחוסר בסנכרון על ה-queue המשותף
- [ ] OS2 — **spurious (fake) wakeups** וההסבר ללולאת ה-`while(!is_work)`; שימוש ב-`ext_lock` (FIFO): הנעילה משוחררת בזמן ה-`wait` ונרכשת מחדש ביקיצה; `consume` מתבצע מחוץ ל-critical section
- [ ] **תרגול:** TQ4 — producer/consumer עם `mutex` + bounded semaphores `empty`(=CAP) ו-`full`(=0): מדוע הגרסה הבאגית נכנסת ל-deadlock (המתנה על semaphore תוך החזקת mutex), והתיקון המינימלי (`wait` לפני רכישת ה-mutex)

---

## 🔐 חלק ג׳ — Kernel Mode, User Mode ו-Signals

### יחידה 7 — מצבי הרשאה וקריאות מערכת  ·  *(OS3 + Rec3 + ex2)*
- [ ] מערכות **monolithic**: תהליך יחיד לאחר boot (בד"כ terminal), גישה ישירה לכל הזיכרון, קוד באגי יכול לגשת לכל מקום, וקריסת תוכנית אחת מקריסה את כל המחשב
- [ ] הפרדה בין **User Mode (Ring 3)** ל-**Kernel Mode (Ring 0)**: קריסת אפליקציה מבודדת לעומת קריסת **driver** ("blue screen"); רכיבי kernel — device/filesystem/graphics drivers, network protocols, crypto, virtualization
- [ ] **CPU Rings** ו-**CPL (Current Privileged Level)**: בד"כ רק ring0 ו-ring3 בשימוש; אכיפה דרך code segment (**IP = code segment + offset**), segments נקבעים ב-boot דרך **page table**; החומרה אוכפת — ניסיון פעולה privileged ב-user mode → exception
- [ ] הבדלי יכולות **kernel** מול **user**: פקודות privileged, גישת זיכרון privileged, ניהול משאבים מול גישה לזיכרון האפליקציה בלבד ובקשת משאבים דרך **syscalls**
- [ ] **system call** — כל גישה ל-kernel; פקודת **`syscall`**/**`int 0x80`** עם קוד ה-syscall ב-**`RAX`**, מעבר Ring3→Ring0; **אין context switch** (אותו thread משנה רק privilege level, התהליך נשאר זהה)
- [ ] **trap** — אירוע **סינכרוני** מהפקודה המבצעת (אותה פקודה = אותו trap): system calls, exceptions, breakpoints (`int 3`); בניגוד ל-**interrupts** האסינכרוניים
- [ ] **trap handler** — נקודת הכניסה ל-kernel ("gatekeeper"): שמירת state, זיהוי סיבה, dispatch לשירות, שחזור, חזרה ל-user
- [ ] **syscall dispatch** ו-**`sys_call_table[]`**: אינדוקס לפי `RAX`; מספרי syscall נפוצים — 0=read, 1=write, 2=open, 3=close, 39=getpid, 57=fork, 60=exit, 62=kill
- [ ] הבחנה **syscall** / **exception** / **SW interrupt** לפי כוונה: בקשת שירות (application logic) / שגיאה לא צפויה (חלוקה באפס, גישה ל-0x0) / טריגר חיצוני-ידני (`int 3`, `int 1`, `int 0x80`, `int 0x20`) — שלושתם מעבירים ל-Ring0
- [ ] מסלול legacy **`int 0x80`**: חיפוש ב-**IDT** (entry 0x80 / 128) → general trap handler (`system_call`) → `syscall_dispatch`; רגיסטרים 32bit — `eax` מספר, `ebx/ecx/edx` ארגומנטים; overhead של חיפוש IDT + trap כללי
- [ ] מסלול מודרני **`syscall`**: קריאה ישירה מ-**`MSR_LSTAR`** → entry ייעודי (`entry_SYSCALL_64`) → dispatch, מדלג על IDT ועל ה-general trap handler; רגיסטרים 64bit — `rax` מספר, `rdi/rsi/rdx/r10` ארגומנטים; `syscall_entry` נקבע ב-boot (`wrmsrl(MSR_LSTAR, ...)`)
- [ ] **`syscall`/`sysret`** (x86-64, מפושט): RCX←RIP, R11←RFLAGS, RIP←handler, CS→ring0, מיסוך flags עם `SFMASK`; `sysret` משחזר וחוזר ל-ring3; **ללא שימוש ב-stack** (מהיר), ואי אפשר לשנות CS ישירות (`mov CS←X` לא חוקי)
- [ ] **`int`/`iret`** (x86, מפושט): דחיפת ESP/EFLAGS/CS/EIP ל-**kernel stack**, CS→ring0, EIP←IDT[X], ESP←kernel stack; `iret` משחזר; **שימוש ב-stack — איטי יותר** מרגיסטרים
- [ ] **kernel stack**: שני stacks per-thread (user ~8MB / kernel 8-16KB); ה-**TSS (Task State Segment)** מחזיק `RSP0` שהחומרה טוענת במעבר privilege; למה לא לסמוך על user stack (בעלות המשתמש על SP, כתובות לא חוקיות/kernel, stack overflow)
- [ ] **hardware interrupts** (אסינכרוני, מכשירי I/O: מקלדת/דיסק/timer) מול **software interrupts** (סינכרוני, פקודת `INT`); taxonomy — כל האירועים דרך IDT; **exceptions** kernel-recoverable (**page fault**, breakpoint) מול signal לתהליך (**SIGSEGV**, **SIGFPE**)
- [ ] **תרגול:** חישוב **system call overhead** (Rec3 תרגיל 1) — 1,200 בלוקים: user compute 960K, trap overhead 840K, kernel work 2.4M, סה"כ 4.2M, overhead 20%, ואופטימיזציית **batching** (10 בלוקים ל-syscall) → 6.54% (שיפור 3.3x)
- [ ] **תרגול:** דוגמת **`Hello.s`** (x64) — `sys_write`/`sys_exit` עם `syscall`, `test %rax`/`js`, בנייה `gcc -nostartfiles -no-pie`; ודוגמת kernel-mode ב-**OS/161** (`int 0x80`, MIPS): `mips_general_handler`→`common_exception`→`mips_trap`→syscall dispatcher, מבנה **trapframe** ומיפוי לרגיסטרי x64
- [ ] **תרגול [HW2]:** חישוב **`int 0x80`** מול **`syscall`** (TQ1) — עלות per-syscall 2,800 מול 2,600 cycles, סה"כ ל-N=10,000, חיסכון 2M cycles, overhead 28.6% מול 23.1%, ונקודת **break-even** עם setup עלות C=10,000 → N=51
- [ ] **תרגול [HW2]:** סיווג מנגנוני כניסה ל-kernel (TQ2, E1–E5) — **system call** / **software interrupt** (`int 3`) / **exception** (חלוקה באפס→SIGFPE); ניתוח "האם נכנסים ל-kernel mode / האם זה context switch"; אמת/שקר — "system call הוא context switch", "`int 3` הוא system call"

### יחידה 8 — Signals וטיפול בחריגות במרחב המשתמש  ·  *(OS3 + Rec3 + ex2)*
- [ ] **Signals** (Linux) ו-**SEH — Structured Exception Handling** (Windows): אפליקציות user-mode רושמות **handler** (function pointer) לתגובה ל-exceptions ברמת ה-OS; ללא handler מותאם → **default action** (סיום/stack trace)
- [ ] **SEH ב-Windows**: `SetUnhandledExceptionFilter` (נשמר ב-**TIB — Thread Information Block**) ו-`AddVectoredExceptionHandler` (וקטור handlers); `EXCEPTION_CONTINUE_EXECUTION` מול `EXCEPTION_CONTINUE_SEARCH`
- [ ] **Signals ב-Linux**: לכל תהליך `task_struct` (ה-PCB) עם **signal handler table** הממפה מספר signal → handler pointer; ללא handler → **default action**
- [ ] מקורות signals: מה-kernel (**SIGSEGV**, **SIGFPE**, SIGCHLD, SIGALRM), מתהליך אחר (`kill`, Ctrl+C→SIGINT, SIGUSR1/2), ל-thread ספציפי (`pthread_kill`, faults); הצינור exception→kernel→signal→user handler
- [ ] שתי רמות ה-state: **process-level** (handler table משותף + process pending queue, נקבע ב-`sigaction`) מול **thread-level** (**signal mask** + thread pending queue, נקבע ב-`pthread_sigmask`); `SIG_DFL`, ו-`SIGKILL`/`SIGSTOP` שאי אפשר לתפוס/להתעלם ("forced")
- [ ] signals נפוצים ו-default action: SIGINT(2), SIGKILL(9), SIGSEGV(11 terminate+core), SIGTERM(15), SIGCHLD(17 ignore), SIGSTOP(19), SIGCONT(18); **default disposition** — terminate / ignore / stop-continue / **core dump**
- [ ] רישום handlers: `signal()` (פשוט אך התנהגות **לא צפויה** — System V מאפס ל-SIG_DFL אחרי הרצה, BSD משאיר מותקן) מול `sigaction()` (מועדף, עקבי); דגלי `sigaction` — **`SA_RESTART`** (restart של syscall שנקטע), **`SA_RESETHAND`**, **`SA_NODEFER`**
- [ ] מסלול **`pause()`** (OS3): עצירת ה-thread, שמירת context, דחיפת **signal frame** (`sigreturn` + placeholder למספר ה-signal), RIP←signal handler (נשמר ב-**PCB לא TCB**); שליחה `pthread_kill(tid, SIGUSR1)`; ללא handler → **TERMINATE**
- [ ] **signal אסינכרוני** מול **המתנה ל-signal**: הרצת handler עם שמירת/שחזור context+RIP; לעומת `sigwait` (`sigemptyset`/`sigaddset`/`pthread_sigmask(SIG_BLOCK)`) — **ללא signal frame וללא שינוי RIP**; `pthread_sigmask` נשמר ב-**TCB** ומשפיע רק על ה-thread הקורא (undo עם `SIG_UNBLOCK`)
- [ ] **עדיפות מסירת signal (POSIX)**: (1) thread ב-`sigwait()` מקבל ראשון וה-signal נצרך ללא handler → (2) לא חסום עם handler רשום → thread eligible אקראי מריץ handler → (3) handler אך חסום בכולם → **pending** → (4) ללא handler וללא sigwait → default action
- [ ] **process-directed** מול **thread-directed**: `kill(pid)` מול `pthread_kill(tid)`; signals **סינכרוניים** (SIGSEGV, SIGFPE, SIGBUS) תמיד thread-directed (ל-thread הגורם); ל-process-directed ה-kernel בוחר thread לא-חסום **אקראי**
- [ ] **signal masks ו-pending**: mask per-thread (נקבע ב-`pthread_sigmask`, נורש ב-`fork`), חסומים נשארים pending; **standard signals** (1–31) כ-**bitmask** — coalescing (רק אחד pending למספר); **real-time signals** (34–64) — **תור** FIFO/priority עם payload דרך `sigqueue`
- [ ] **signal delivery flow**: generation → pending → delivery (בדיקת kernel בחזרה ל-user) → handling; signal יכול לקטוע syscall חוסם (`read`) → החזרת `-1` עם `errno=EINTR`, ו-**`SA_RESTART`** מפעיל מחדש
- [ ] **signals תוך כדי הרצת handler**: כברירת מחדל ה-signal המפעיל נחסם, אך signals אחרים יכולים לקטוע את ה-handler → בעיות **reentrancy** ושחיתות state משותף
- [ ] **async-signal-safety**: הגדרה (בטוח לקריאה מתוך handler גם אם קטע את אותה פונקציה); רוב הפונקציות **אינן** בטוחות (locks/buffers/static); למה `printf` לא בטוח (deadlock על lock פנימי / שחיתות buffer) — ולכן `write()` ב-handler
- [ ] **צירוף המסכות**: `sa_mask` מוסיף signals ל-mask של ה-thread למשך ה-handler; **הנוסחה** `handler_mask = original_mask ∪ sa_mask ∪ {S}`, ובחזרה `current_mask := original_mask`; עם **`SA_NODEFER`** ה-`{S}` לא מתווסף → `handler_mask = original_mask ∪ sa_mask` (re-entrant)
- [ ] **תרגול:** דוגמת **`seh_example.c`** (חלוקה באפס ב-Windows) — מה ה-output, ומה קורה אם מסירים את `Rip++` או את `SetUnhandledExceptionFilter`
- [ ] **תרגול:** דוגמת **signal mask** (Rec3) — original `{SIGINT}`, `sa_mask` `{SIGTERM}`, נמסר SIGUSR1: בזמן handler `{SIGINT,SIGTERM,SIGUSR1}`, אחרי `{SIGINT}`; ודוגמת `sigaction` תקינה (`write` לא `printf`, `SA_RESTART`)
- [ ] **תרגול:** **signal masking כסנכרון** — תרחיש deadlock (`mutex_lock` בתוך handler → self-deadlock) והפתרון: מיסוך `sigprocmask` סביב critical section
- [ ] **תרגול:** **Exercise 2 — Logging** (Rec3): `log_buf`+`rwlock`, handler/writer/reader; interleaving מסוכן (self-deadlock כש-SIGUSR1 מגיע ל-thread שמחזיק lock), טבלת 6 interleavings, והתיקון במיסוך signals
- [ ] **תרגול:** **Exercise 3 — Sensor Monitor** (Rec3): handlers של SIGHUP/SIGUSR1; **Danger 1** — nested handler (SIGUSR1 באמצע SIGHUP) → deadlock + state לא עקבי; **Danger 2** — signal ל-thread שמחזיק lock; תיקונים — block SIGUSR1 ב-`sa_mask`, ומיסוך signals סביב ה-locks
- [ ] **תרגול [HW2]:** מסירת signal בין threads (TQ2) — `SIGUSR1` נצרך ע"י T2 ב-`sigwait` (עדיפות על ה-handler של T1); אמת/שקר — ה-handler לא חייב לרוץ ב-thread שקרא ל-`signal`, `sigwait` לא מריץ handler, ו-SIGFPE ללא handler → default (סיום)
- [ ] **תרגול [HW2]:** **signal delivery תוך החזקת mutex** (TQ3) — `incrementer`/`reader`/`handler` על `counter`; טבלת 6 סדרי critical sections (ערכים 1/0/-1, סופי תמיד 0); deadlock כאשר ה-thread המקבל כבר מחזיק את ה-mutex הלא-רקורסיבי; מניעה במיסוך SIGUSR1 ב-**שני** ה-threads

### יחידה 9 — Threading במרחב המשתמש (setjmp/longjmp ו-uthreads)  ·  *(OS3 + Rec3 + ex2)*
- [ ] **`setjmp`/`longjmp`**: `setjmp` שומר את context ה-thread ב-`jmp_buf` (RIP, RSP...) ומחזיר **0**; `longjmp` משחזר את ה-context ומחזיר **ערך שונה מ-0** ("checkpoint &amp; restore")
- [ ] **`sigsetjmp`/`siglongjmp`**: אם משתמשים ב-`longjmp` ליציאה מ-handler, ה-**signal mask** עלול לא להישמר (תלוי-פלטפורמה); `sigsetjmp(env, savemask)` שומר את המסכה (כש-savemask≠0) ו-`siglongjmp` משחזר אותה
- [ ] **user-mode threading**: "קפיצה" של thread עם `setjmp`/`longjmp` שמחקה concurrency של ה-kernel; **יתרונות** — ללא overhead של system call ב-context switch, state קטן יותר, מעבר מהיר; **חסרונות** — ה-OS לא מודע ל-threads, **ללא parallelism אמיתי** (כל ה-user threads על kernel thread יחיד), מורכב לניהול
- [ ] **תרגול:** ארבע דוגמאות `setjmp`/`longjmp` ב-OS3 — זרימת if/else, handler ל-SIGFPE + `longjmp`, `longjmp` אל **stack frame מת** (פונקציה שכבר חזרה), ו-`longjmp` **בין threads** (undefined behavior); לכל אחת "מה ה-output?"
- [ ] **תרגול:** שחזור שגיאה עם `setjmp`/`longjmp` (Rec3) — `divide` שקורא `longjmp` על חלוקה באפס, ו-`main` עם checkpoint (`setjmp==0`)
- [ ] **תרגול:** **Exercise 4 — Database Transaction Rollback** (Rec3): אנלוגיית SQL (`BEGIN`/`ROLLBACK`/`COMMIT`); ה-handler המסוכן (`siglongjmp` בזמן החזקת lock → **lock orphaned** + auditor ב-deadlock); טבלת interleaving (6 תרחישי הגעת SIGUSR1) והתיקון — cleanup לפני הקפיצה (שחזור ערכים, `unlock`, ואז `siglongjmp`)
- [ ] **תרגול [HW2]:** **switch_to bug** (TQ4) — `current_uthread` מעודכן **אחרי** ה-`siglongjmp` (שלא חוזר) במקום לפניו → נשמר context של ה-uthread הלא-נכון; התיקון, ה-output של התוכנית, משמעות `sigsetjmp==0` מול ≠0, ולמה שני cores לא נותנים parallelism אמיתי
- [ ] **[HW2] ספריית uthreads (Part B)** — static library ללא `main()`; uthread = user-level thread על תהליך יחיד (לא pthread); main uthread עם tid=0; הקצאת ה-tid הפנוי הקטן ביותר; עד `MAX_THREAD_NUM` (100)
- [ ] **[HW2] state diagram**: `READY` / `RUNNING` / `BLOCKED` (+ Deleted); מעברים `spawn`→READY, `schedule`, `preempt`, `block`, `resume`, `terminated`
- [ ] **[HW2] scheduler Round-Robin (RR)**: **quantum** קבוע לכל uthread ב-RUNNING; תנאי preemption (פקיעת quantum / block / sleep / terminate); ניהול תור READY (הוספה לסוף, ללא כפילויות); reset של הטיימר ל-quantum מלא בכל החלפה
- [ ] **[HW2] virtual time מול wall-clock**: virtual time מודד CPU time של התהליך (מתקדם רק כשרץ), ממומש ב-`setitimer` עם `ITIMER_VIRTUAL` ו-`SIGVTALRM`; wall-clock אינו בשימוש לתזמון
- [ ] **[HW2] ספירת quantums**: מתחילה ב-1 מיד אחרי `uthread_init` (ה-main מתחיל quantum ראשון); כל מעבר ל-RUNNING מגדיל את הסופר הכללי ואת סופר ה-uthread; uthread חדש עם 0 עד שרץ; ל-RUNNING נכלל ה-quantum הנוכחי
- [ ] **[HW2] block/sleep**: `uthread_block` מול `uthread_sleep`; `uthread_resume` מנקה רק את החסימה המפורשת; main (tid=0) אסור לו לישון; `num_quantums<=0` = שגיאה
- [ ] **[HW2] API + דרישות מימוש**: `uthread_init/spawn/terminate/block/resume/sleep/get_tid/get_total_quantums/get_quantums`; הגנה על עדכוני state ב-**signal masking**; איסור `malloc` (מערכים סטטיים); אין הדפסה ל-`stdout`; הודעות שגיאה ל-`stderr` (`system error:` / `thread library error:`)

---

## ⏱️ חלק ד׳ — תזמון (Scheduling)

### יחידה 10 — תזמון מעבד (CPU Scheduling)  ·  *(OS4 + Rec4)*  🧮 *היחידה החישובית ביותר*
- [ ] מהו מתזמן המעבד (**CPU Scheduler**) ומתי מתרחש **context switch** (yield / חסימה על primitive סנכרון / פסיקת טיימר) — דיאגרמת מצבי thread: New, Ready, Running, Wait, Terminated
- [ ] הבחנה בין **mechanism** (שמירת מצבים, החלפת הקשר, מדידות) ל-**policy** (איזה thread רץ ומתי)
- [ ] יעדי התזמון: היענות (**responsiveness**), הוגנות (**fairness** — ללא **starvation**), ניצול מעבד (**CPU utilization**)
- [ ] הבחנה בין thread חסום-קלט/פלט (**I/O-bound**) ל-thread עתיר-חישוב (**CPU-bound**), ולמה רוצים "לתגמל" (reward) threads מסוג I/O-bound
- [ ] **תזמון פולש (preemption)** ו-**quantum**: quantum ארוך מדי → פוגע בהיענות; קצר מדי → overhead של החלפות הקשר; טווח טיפוסי 10ms–100ms
- [ ] הבחנה בין תזמון **Off-Line** (מספר וגודל העבודות ידועים) ל-**On-Line** (אין מידע על הגעה/אורך/התנהגות)
- [ ] **FCFS** (offline): הרצה לפי סדר הגעה, אפקט השיירה (**convoy effect**) — עבודות קצרות נתקעות אחרי ארוכות
- [ ] **SJF** (offline): הקצר-ביותר-קודם, אופטימלי למזעור זמן המתנה ממוצע, גרסה פולשת **SRTF** משפרת עוד, אך סיכון ל-**starvation** של עבודות ארוכות
- [ ] אמידת אורך ה-CPU burst הבא כדי ליישם SJF בפועל: הנוסחה $\tau_{n+1} = \alpha\, t_n + (1-\alpha)\,\tau_n$, משמעות $\alpha$ (0 → מתעלם מהמדידה החדשה, 1 → רק המדידה האחרונה) ומגבלותיה
- [ ] **Random** (online): בחירת thread אקראית — ייתכן thread שלא נבחר כלל
- [ ] **Round Robin** (online): מנה שווה מחזורית לכל thread; חסם ההוגנות — thread לא ממתין יותר מ-$(n-1)\times q$
- [ ] **MLFQ** (Multi-Level Feedback Queues): תורי עדיפות 0..N (0 הגבוה), thread חדש נכנס לתור 0; חסימה/yield → קידום (promote), ניצול ה-quantum המלא → הורדה (demote); בכל רמה quantum קבוע (עולה ככל שהעדיפות יורדת), וסיכון ל-**starvation** ללא boosting
- [ ] **CFS** (Completely Fair Scheduler): Ingo Molnár, שיפור התזמון ל-$O(1)$ תוך שמירה על הוגנות (בעקבות עבודת Con Kolivas; לפני כן המתזמן היה $O(n)$)
- [ ] **RSDL** (Rotating Staircase Deadline Scheduler): Con Kolivas; **RR_INTERVAL** (ברירת מחדל 6ms), **thread quota**, **queue quota** = סכום מכסות ה-threads בתור, וריצה ב-**epochs** שמתאפסים; חוקי ההורדה: מיצוי מכסת thread → הורדה עם רענון מכסה, מיצוי מכסת תור → הורדת כל ה-threads בתור יחד, חסימת I/O → נשאר באותה רמה
- [ ] **תרגול:** ריצות מעבד (Gantt) של **FCFS** ו-**SJF** ב-OS4 — חישוב זמן המתנה ממוצע, זמן תגובה ממוצע ו-throughput לאותן שלוש עבודות (6/3/1), והשוואה בין הסדרים; ב-Rec4 דוגמאות Gantt נוספות
- [ ] **תרגול:** ריצת **Round Robin** ב-Rec4 (quantum, סדר T1,T3,T2, מחזוריות עם חזרה לסוף התור) וריצת Random/RR ב-OS4 עם מעקב Ready/Wait צעד-אחר-צעד
- [ ] **תרגול:** מעקב **MLFQ** מלא — ב-OS4 טבלת 12 צעדים עם 4 threads, וב-Rec4 דוגמת 6 צעדים עם $t_1$ CPU-bound ו-$t_2$ I/O-bound (חוסם אחרי 1ms) הממחישה promote/demote (Q0=2ms, Q1=4ms, Q2=8ms)
- [ ] **תרגול:** מעקב **epoch** מלא של **RSDL** — ב-OS4 מהלך 11 צעדים עד "end of EPOCH" (5 threads, מכסות 6, כולל מיצוי queue-quota והורדה קבוצתית), וב-Rec4 walkthrough בן 9 צעדים (RR_INTERVAL=3ms, Q0=18/Q1=6/Q2=6) הכולל חסימת I/O, מיצוי מכסת תור, סוף epoch ו-reset מלא
- [ ] **תרגול:** מצבורי נכון/לא-נכון ב-Rec4 על מושגי תזמון כלליים, preemption/quantum, ו-FCFS/SJF/SRTF/RR/MLFQ/RSDL (למשל: FCFS לא אופטימלי; ב-RSDL חסימת I/O אינה מורידה רמה; מיצוי מכסת תור מוריד את כל ה-threads)
- [ ] **תרגול:** שאלת התכנון הפתוחה של הרחפן האוטונומי (Autonomous Drone) — משימות OD/RC/BM עם deadline, בחירת **MLFQ** + preemption כדי להבטיח ש-OD עומד ב-50ms, ולמה לא RSDL
- [ ] **תרגול:** שאלת ביצועים עם I/O הנפתרת בארבע דרכים — 3 threads (הגעות 0/1/2, בורסטים ו-I/O) המחושבים ב-**FCFS**, ב-**Round Robin** (quantum=2ms), ב-**MLFQ** מותאם וב-**RSDL** מותאם; חישוב זמן סיום (completion), **turnaround** (completion−arrival) ו-**waiting** (turnaround−burst−IO) לכל thread
- [ ] **תרגול [HW3]:** וריאנט ה-**MLFQ** מ-TQ1 (Q0=10ms/Q1=20ms/Q2=40ms) — Thread A באמת CPU-bound מול Thread B שמתחזה ל-I/O-bound ע"י `yield()` אחרי 9.9ms מכל quantum; ניתוח מצב היציב (A ב-Q1 ומורעב, B נשאר ב-Q0), אחוז זמן ה-CPU של A, מה הווריאנט מניח בטעות, והאם B יורד רמה אי-פעם

---

## 💾 חלק ה׳ — זיכרון וזיכרון וירטואלי

### יחידה 11 — זיכרון פיזי והתפתחותו  ·  *(OS5 + Rec5)*
- [ ] היסטוריית ה-**RAM** — מ**זיכרון סרטים** (גישה סדרתית) ל-**Random Access Memory**: Viehe (1947), An Wang (1949), Jay Forrester (1951 — המימוש הראשון)
- [ ] סוגי זיכרון נדיף: **DRAM** (רענון כל 10-100ms, איטי/זול), **SRAM** (זוכר כל עוד יש חשמל, מהיר/יקר — caches ו-registers), **SDRAM** (רענון מסונכרן לשעון)
- [ ] עידן ה-**MS-DOS**: מרחב כתובות 1MB ($2^{20}$); **Conventional memory** (0-640KB), **UMA** (640KB-1MB), מגבלת ה-640k; מעבר ל-1MB: **HMA**, **Extended Memory** (XMS), **Expanded Memory** (EMS, bank switching)
- [ ] מעבר מתוכנית יחידה ל-**multitasking**; טעינת תהליכים לבלוק פיזי רציף; היווצרות חורים בסיום תהליך; הצורך בזיכרון **רציף** (Word עם 5000 ints רצופים)
- [ ] **External fragmentation** — זיכרון פנוי *בין* תהליכים; **Internal fragmentation** — זיכרון פנוי *בתוך* תהליך
- [ ] זיכרון פיזי כ**מרחב כתובות רציף** מנוהל ע"י ה-OS; חלוקה ל-**frames** בגודל קבוע או **partitions** משתנים; בעיות הגישה הישירה: אין **isolation**, אין **relocation**, fragmentation, ריבוי-תוכניות מוגבל *(Rec5)*

### יחידה 12 — זיכרון וירטואלי ותרגום כתובות  ·  *(OS5 + Rec5)*
- [ ] רעיון ה-**virtual memory**: שכבת indirection שנותנת לכל תהליך את כל מרחב הכתובות; ה-OS מתרגם *virtual address* → *physical address*; אותה VA בתהליכים שונים ממופה לכתובות פיזיות שונות (isolation)
- [ ] גדלי מרחב כתובות: 32-bit → 4GB ($2^{32}$), 64-bit → 16EB ($2^{64}$), בפועל 48 ביט → 256TB לכל תהליך; הפריסה (Code/Data/Heap/Stack) מוגדרת ע"י ה-executable (**PE**/**ELF**/**Mach-o**)
- [ ] סוגי זיכרון: **anonymous** (malloc/stack/mmap, מיפוי עצל) מול **file-backed** (PE/ELF); **Buffered I/O** (fread, 2 עותקים) מול **Memory-Mapped I/O** (mmap, zero-copy); מונחים: **Resident**, **Dirty**, **Evict**, **SWAP**, **Page Cache**
- [ ] נגישות: **Private** מול **Shared**; **Copy-on-Write (COW)** — שיתוף עד הכתיבה הראשונה (אופטימיזציה ל-fork)
- [ ] **Access Control**: הרשאות Read/Write/Execute; הפרה → exception; קריאת פלט **pmap** (זיהוי anonymous מול file-backed)
- [ ] למה לא לתרגם דרך ה-kernel בכל גישה: לא בטוח, חושף את הפיזי, ואיטי מדי (bulk translation מנצל locality) *(Rec5)*
- [ ] **MMU**: מנגנון חומרה — CPU שולח VA, MMU מחזיר PA, מגובה ב-**TLB**; מנגנון מול מדיניות (ה-OS קובע policy, ה-MMU מבצע); exception ו-**Segmentation Fault** / Access Violation
- [ ] **base and bound** (UNIVAC EXEC II 1962): base פיזי + bound offset; VA + Base = PA ובדיקת bound; מהיר אך דורש זיכרון רציף וסובל מ-fragmentation
- [ ] **segmentation** (Multics, Windows 3.x): **segment table** (Base/Limit/Permissions), Seg#+Offset, offset>limit → Seg Fault; מקטין external fragmentation אך internal נשאר
- [ ] **CAM** (Content Addressable Memory): חיפוש טבלה שלמה ב-clock cycle אחד; Binary/Trinary; מהיר אך יקר; המעבר ל-**TLB** ("tagged CAM" עם PID) וקיבוץ כתובות ל-**pages**

### יחידה 13 — Paging, טבלאות דפים וה-TLB  ·  *(OS5 + Rec5)*
- [ ] **page** = מקטע רציף של תהליך; **page table** לכל תהליך (נגיש מה-PCB), מאוחסן ב-RAM; **PTBR** מצביע לבסיס הטבלה; ה-MMU ניגש לטבלאות ישירות
- [ ] **PTE** (Page Table Entry): **PPN/PFN**, **Valid bit**, **Present bit**, הרשאות **RWX**, **Dirty**, **Accessed**, Private/Shared, User/Supervisor
- [ ] פיצול ה-VA: page 4k → 12 ביט **offset**, שאר הביטים = **VPN**; **page table כמערך**: אינדקס = VPN, גישה ב-O(1), אך חייב להיות רציף → **multi-level page table**
- [ ] **תרגול:** תרגום כתובת ידני — **TLB hit** (חיפוש VPN, שרשור PPN+offset) מול **TLB fault** (VPN לא נמצא)
- [ ] **page-table walk** ו-**page fault**: TLB miss → אינדוקס page_table[VPN]; אם Valid=0 → page fault, trap ל-OS, טעינה מ-file/swap ל-frame פנוי, עדכון ה-PTE, הכנסת שורה ל-TLB
- [ ] ניהול/הסרת שורות TLB: פקודת **TLBI** ב-ARM; ב-context switch התרגומים לא תקפים → **TLB flush** (יקר) מול תיוג **ASID/PCID** *(Rec5)*
- [ ] **multi-level page table**: פיצול ה-VPN לרמות, PTBR→$L_n$, כל רמה מצביעה לרמה נמוכה, ביצועים ∝ מספר הרמות; ניתן לפנות רמות ולטעון ב-page fault
- [ ] **תרגול:** page-table walk דו-שלבי (`0x00403004`, פיצול 10-10-12); חישוב גודל טבלת דפים (32-bit, 4KB, PTE 4B → 4MB → מוטיבציה למולטי-לוול); חישוב **VPN**/**offset**; walk עם **TLB** (הדגמת **TLB miss ≠ page fault**) *(Rec5)*
- [ ] רמות **cache**: Registers / L1 / L2 / L3 / Main Memory / Disk וזמני הגישה; L3 משותף בין cores; **multi-level TLB**; אופטימיזציה: לא להקצות פיזית ב-malloc אלא רק בגישה בפועל

### יחידה 14 — Swapping והחלפת דפים  ·  *(OS5 + Rec5)*  ⚠️ *התרגול הוא המקור העיקרי*
- [ ] **swap in/out**: כשנגמר הזיכרון הפיזי ה-OS מפנה דפים ל-swap file; swap in מתרחש ב-page fault; שלב-אחר-שלב (כתיבה לדיסק → שחרור ה-frame → ביטול ה-PTE, וההפך); **pre-allocation** של קובץ ה-swap
- [ ] **swap space**: אזור דיסק ל-**anonymous pages** (heap/stack); file-backed נטען מחדש מהקובץ; מצבי דף **Clean** (ניתן לזרוק) מול **Dirty** (חייב להיכתב חזרה) *(Rec5)*
- [ ] **demand paging** (lazy loading): דפים נטענים רק בגישה הראשונה (page fault), מאפשר תוכניות גדולות מהזיכרון הפיזי *(רק ב-Rec5)*
- [ ] בחירת קורבן (victim): **random** מול **least recently used**
- [ ] **LRU**: מימוש ב-**linked list** דו-כיווני (front=MRU, tail=LRU); הזזת דף שניגשו אליו לראש, פינוי מהזנב; חיפוש O(n)
- [ ] **Clock** (approx-LRU): מבנה מעגלי של PTEs + **reference bit** שה-MMU מעדכן; finger pointer — bit=1→אפס והמשך, bit=0→evict; יעיל מ-LRU (עדכון רק ב-page fault)
- [ ] **FIFO** להחלפת דפים: הדף הוותיק ביותר מפונה (תור), ללא התחשבות בשימוש *(רק ב-Rec5)*
- [ ] **Belady's Anomaly**: ב-**FIFO** יותר frames עלולים לגרום ליותר page faults (מחרוזת 1,2,3,4,1,2,5,1,2,3,4,5: 3 frames→9 faults, 4 frames→10 faults) *(רק ב-Rec5)*
- [ ] **working set**: קבוצת הדפים שנגישו ב-Δ האחרון (locality); אם אינו נכנס ל-frames → **thrashing** — יותר זמן על page faults מעבודה מועילה *(רק ב-Rec5)*
- [ ] **EAT** (Effective Access Time): נוסחה $(1-p)\big(\alpha T_{hit}+(1-\alpha)T_{miss}\big)+p\cdot T_{fault}$; חישוב עם TLB hit/miss ועם page-fault rate *(רק ב-Rec5)*
- [ ] **תרגול:** הרצת **LRU** / **FIFO** / **Clock** על מחרוזות ייחוסים וספירת **page faults**; הדגמת **Belady** בהשוואת 3↔4 frames; דפוסי גישה למערך 4D (row-major) והשפעת locality *(Rec5)*
- [ ] **[HW3]** demand paging והחלפת דפים (ex3 TQ2): page fault כאשר Valid=0; סימולציית **LRU** (10 faults) מול **Clock** (9 faults) על 1,2,3,4,1,2,5,1,2,3,4,5; eviction של דף **dirty** דורש כתיבה ל-swap (בניגוד ל-clean)

---

## 🗄️ חלק ו׳ — מערכות קבצים ואחסון

### יחידה 15 — חומרת אחסון  ·  *(OS6 + Rec6)*
- [ ] **HDD**: לא-נדיף, אחסון מגנטי על פלטות מסתובבות; חלקים נעים ⇒ איטי; זול, קיבולת גדולה; רכיבים — **Platters**, **Spindle** (4200–15,000 RPM), **Head**, **Actuator**, **IDE**/**SATA connector**, **Jumper block**
- [ ] גאומטריית **HDD**: **Track** (מסלול), **Sector** (512–4096 בתים; יחידת הקריאה/כתיבה המינימלית של הראש), **Cylinder** (אותו מסלול לאורך כל הפלטות)
- [ ] **NAND/NOR flash**: לא-נדיף, נתונים ב-**Floating Gate Transistors**, מספר מוגבל של פעולות עד בלאי
- [ ] **SSD**: **flash memory**, ללא חלקים נעים ⇒ **random-access** מהיר ועמיד; קטן ויקר יותר מ-HDD; **wear leveling** ו-**garbage collection**; ארגון **pages** (יחידת כתיבה, ~4-16KB) ו-**blocks** (~128 pages); צריך למחוק block לפני כתיבה מחדש
- [ ] אמולציית סקטורים ב-**SSD**: כתיבת 512 בתים משפיעה על page שלם (4KB) ⇒ **performance hit**
- [ ] **Sector vs Block/Cluster**: **Sector** = יחידת האחסון הפיזית הקטנה ביותר; **Block/Cluster** = יחידת ההקצאה הקטנה ביותר של מערכת הקבצים; הקצאה תמיד ב-blocks שלמים ⇒ **internal fragmentation** (קובץ 1KB ב-block 4KB מבזבז 3KB)

### יחידה 16 — הקצאת קבצים וספריות  ·  *(OS6 + Rec6)*  📂 *היחידה הצפופה ביותר בחלק זה*
- [ ] **Contiguous allocation**: קובץ = ריצה של blocks עוקבים (start + length); פשטות ומעט seeks, אך **external fragmentation** וצורך לדעת גודל מראש; שימוש: CD-ROM, IBM OS/360
- [ ] **Linked-list allocation**: כל block מכיל data + מצביע ל-block הבא; ללא external fragmentation, אך **random access** איטי + overhead של מצביע; שימוש: MS-DOS
- [ ] **FAT** (File Allocation Table): גרסה טבלאית של linked-list — המצביעים בטבלה נפרדת ב-RAM ⇒ random access מהיר; **FAT32**: entry 4 בתים, 28 ביט לכתובת, עד 2^28 blocks; טרייד-אוף גודל block מול internal fragmentation
- [ ] **NTFS / MFT** (Master File Table): מסד נתונים; רשומה 1KB לכל קובץ; 16 רשומות ראשונות שמורות ($MFT/$LogFile/root ב-#5); **resident** (קבצים קטנים בתוך ה-MFT) מול **non-resident** (**data runs**); אינדקס ספרייה ב-**B⁺-Tree**
- [ ] **inode** (\*nix): מיקום קבוע שנקבע ב-**format**, inode אחד לכל קובץ; **content block** מול **indirection block**; **multi-level inode** ב-**EXT4**: 12 מצביעים ישירים (48KB), **single** (4MB), **double** (4GB), **triple** (4TB) — 15 מצביעים בסה"כ
- [ ] טבלאות **inode** סטטיות ב-**EXT4**: השטח מוקצה מראש ב-format ⇒ מספר מקסימלי קבוע של inodes; דוגמת "No space left on device" למרות שטח פנוי
- [ ] **Partitions**: חלוקת דיסק למחיצות, כל אחת מפורמטת למערכת קבצים שונה; **FORMAT** = יצירת מבנה מערכת הקבצים; **MOUNT** (\*nix) = חיבור מחיצה/התקן לספרייה; ב-Windows אות כונן
- [ ] **Directory** = קובץ מיוחד עם טבלת ספרייה; sub-directories → עץ; "." (נוכחית) ו-".." (הורה); לכל תהליך current directory ב-**PCB**; נתיב יחסי מול מוחלט
- [ ] איתור קובץ לפי שם: **FAT** — directory entry 32 בתים + FAT chain עד EOF; **NTFS** — MFT ואז **B⁺-Tree** של הספרייה; **EXT4** — ספרייה ממפה שם→inode number, root = inode 2
- [ ] **Journaling ב-NTFS** *(רק ב-Rec6)*: **write-ahead logging** ל-`$LogFile` — כל שינוי metadata נרשם ל-journal לפני הכתיבה; מחזור Begin → Write → Commit; שחזור ב-restart (**Redo/Undo**); **metadata-only**
- [ ] **Defragmentation**: קבצים מתפזרים ⇒ seeks נוספים ב-**HDD**; ב-**SSD** מיותר ואף **מזיק** (כתיבות מיותרות ⇒ בלאי)
- [ ] מאפייני קובץ לפי מערכת: **FAT** (ב-directory entry, ללא הרשאות), **EXT4** (ב-**inode**, הרשאות r/w/x), **NTFS** (ב-MFT: **ACL**/**ACE** עם SID, **alternate data streams** `[name]:$DATA`)
- [ ] **תרגול:** מעקב שרשרת **FAT** (3→4→5→6→-1); גודל טבלת **FAT32** (1TiB, block 4KiB ⇒ 1GiB ב-RAM); האם FAT32 פורס דיסק 8TiB (לא, 2^31 > 2^28); נפחים מקסימליים
- [ ] **תרגול:** **I/O ב-Linked-list** מול **contiguous** (1/5/10 מול קפיצה ישירה); **I/O ב-FAT** (RAM ⇒ 1, דיסק ⇒ 6/11); **I/O ב-NTFS** (נתיב עם B-tree, resident/non-resident ⇒ 8/7/5)
- [ ] **תרגול:** איזה מצביע **inode** מכסה offset (300KiB ⇒ **single-indirect**); **internal fragmentation** ב-EXT4; תרגיל **inode** מקיף (Rec6: 200GiB, block 4KiB, inode 256B, 10 direct+2 single+2 double+1 triple ⇒ גודל טבלת inodes וגודל קובץ מקסימלי)
- [ ] **[HW3]** תרגיל indirect blocks (TQ3): קובץ 5GiB, 12 ישיר + single + double + triple, block 4KiB, מצביע 4B; ישיר+single+double מכסים 1,049,612 ⇒ **triple-indirect נדרש**
- [ ] **[HW3]** מימוש **fs.c** ("OnlyFiles"): דיסק וירטואלי 10MB, block 4KB, 2560 blocks; פריסה — **Superblock** (0), **Block Bitmap** (1), **Inode Table** (2–9, 256 inodes), **Data Blocks** (מ-10); 12 מצביעים ישירים ⇒ מקסימום 48KB; API `fs_format/mount/unmount/create/delete/list/write/read`; חובה syscalls, אסור stdio/`mmap`/`malloc`

### יחידה 17 — Boot, מחיצות וקושחה  ·  *(Rec6 בלבד)*  ⚠️ *נלמד רק בתרגול*
- [ ] **Motherboard**, **ROM** (זיכרון לא-נדיף השומר firmware), **Firmware** (הקוד הראשון שרץ בהדלקה — מאתחל חומרה ומעביר שליטה ל-bootloader)
- [ ] **BIOS**: firmware ישן; **POST**, מחפש דיסק אתחול, קורא 512 בתים ראשונים (**MBR**), מריץ bootloader (446 בתים); מגבלות: **16-bit real mode**, ללא אימות אבטחה, קשור ל-MBR (עד 2TB, 4 מחיצות primary)
- [ ] **UEFI**: מחליף מודרני (~2012+); **32/64-bit**, **boot manager** משלו, קורא **GPT**, **Secure Boot** (רק bootloaders חתומים)
- [ ] תהליך ה-**Boot** — השוואה: **BIOS** (Find boot disk → Read MBR → Execute bootloader → Load kernel, 16-bit, ללא אימות) מול **UEFI** (Read GPT → Find boot partition → Execute `.efi` → Load kernel, 32/64-bit, Secure Boot)
- [ ] **MBR** (Master Boot Record): פריסת 512 בתים — bootloader code (446B), partition table (64B=4×16B), boot signature (2B, `0x55AA`); עד 4 מחיצות primary, עד 2TB
- [ ] **GPT** (GUID Partition Table): header גיבוי בסוף הדיסק, עד 128 מחיצות, מעל 2TB, **CRC32**, **Protective MBR**; **ESP** — מחיצת **FAT32** קטנה המחזיקה קבצי `.efi`

### יחידה 18 — קישורים, מתארי קבצים וצינורות  ·  *(OS6 + Rec6)*
- [ ] **Hard link**: שני נתיבים = אותו קובץ (אותו **inode**/**MFT record**); רק באותה מחיצה; לא לספריות; מחיקה רק כשכל הנכנסות נמחקות (reference count); `ln`, `mklink /H`
- [ ] **Symbolic link** (soft link): קובץ שמצביע לנתיב אחר; מחיקת המקור ⇒ הקישור נשבר; עובד עם ספריות; `ln -s`; ב-Windows **reparse points** ו-**Junction** (`mklink /D`/`/J`)
- [ ] **File Descriptor** (\*nix) / **HANDLE** (Windows): מזהה לקובץ פתוח; רשימת קבצים פתוחים ב-**PCB**; ה-cursor נשמר ב-FD; "הכל קובץ" (sockets/pipes/terminals/devices); 0=**stdin**, 1=**stdout**, 2=**stderr**; **File Descriptor Table** → **Kernel File Table** → disk blocks
- [ ] פעולות קבצים ב-\*nix: **open** (flags O_RDONLY/O_WRONLY/O_RDWR/O_CREAT/O_APPEND), **close**, **read**, **write**, **lseek** (SEEK_SET/END/CUR); **FILE\*** = wrapper מעל fd
- [ ] **תרגול (רק ב-Rec6):** **File Descriptors אחרי fork()** — fork משכפל את טבלת ה-FD (שני FDs, אחד בכל תהליך) המצביעים לאותה כניסה ב-kernel file table ⇒ **offset משותף**; `close` בילד לא סוגר את של האב; סדר הכתיבה עם `wait()` דטרמיניסטי
- [ ] **Named Pipe** (FIFO): קובץ מיוחד (עם inode) ל-**IPC** בין תהליכים **לא-קשורים**; `mkfifo`; **Anonymous pipe**: ערוץ בין אב-ילד, `pipe()` (fd[2]), חד-כיווני, דוגמה `ls | grep`
- [ ] **[HW3]** קישורים ומחיצות (TQ3, שתי מחיצות ext4): `mv` באותה מחיצה (0 בתים) מול בין מחיצות (העתקת כל התוכן); **hard link לא יכול לחצות מחיצות**; **symbolic link** נשבר אם המקור נמחק

### יחידה 19 — RAID  ·  *(OS6 + Rec6)*
- [ ] **RAID** (Redundant Array of Independent Disks): ביצועים / **redundancy** / קיבולת; **Software** מול **Hardware RAID**; מוטיבציה: **AFR** (HDD 1.64%, SSD 0.98%)
- [ ] **RAID 0** (striping): קיבולת = סכום הדיסקים, ביצועים עולים, **ללא redundancy**
- [ ] **RAID 1** (mirroring): שורד כשל דיסק אחד; הפסד 50% קיבולת; קריאה מהירה, כתיבה כפולה
- [ ] **RAID 10** (1+0): שיקוף ואז striping; ביצועים גבוהים; 50% לשיקוף; שורד כל עוד לא נכשל זוג משוקף שלם
- [ ] **RAID 4**: RAID 0 + דיסק **parity** ייעודי; redundancy עד דיסק אחד; **דיסק ה-parity הוא צוואר בקבוק** בכתיבה
- [ ] **RAID 5**: **parity** מבוזר בין הדיסקים (פותר את צוואר הבקבוק); קיבולת ו-redundancy כמו RAID 4
- [ ] **RAID 6**: **double parity** — P (**XOR**) ו-Q (**weighted XOR**) ⇒ שורד כשל של 2 דיסקים; כתיבה איטית וקיבולת מופחתת
- [ ] **XOR parity** לשחזור: `parity = A1⊕A2⊕A3`; שחזור דיסק אבוד = XOR של ששרדו + ה-parity; תכונות `A⊕A=0`, `A⊕0=A`
- [ ] **תרגול:** חישוב ושחזור **XOR parity** (RAID 5, 4 דיסקים 8-bit)
- [ ] **תרגול:** קיבולת RAID ועמידות (6 דיסקים × 2TiB): RAID 0 = 12TiB/0, RAID 1 = 6TiB/5, RAID 5 = 10TiB/1, RAID 6 = 8TiB/2, RAID 10 = 6TiB/1–3; נוסחאות N·S, (N/2)·S, (N-1)·S, (N-2)·S
- [ ] **תרגול:** שחזור **RAID 6 weighted-XOR** מעל **Galois Field** GF(2^8) — `Q = D₁ ⊕ (D₂×2¹) ⊕ …`; **irreducible polynomial** `0x11B`; דוגמה מלאה עם shift ו-XOR

---

## 🔗 חלק ז׳ — קובצי הרצה, קישור ווירטואליזציה

### יחידה 20 — קובצי הרצה וקישור (Executables &amp; Linking)  ·  *(OS7 בלבד)*  ⚠️ *נלמד רק בהרצאה — מקור יחיד, אין גיבוי בתרגול*
- [ ] Hello World בלי `main()` — נקודת כניסה **`_start`** (ניתן לשנות עם `--entry`); ה-OS לא דורש `main`, ויוצאים ב-syscall (לא `ret` מנקודת הכניסה)
- [ ] תפקיד ה-**linker (`ld`)** — resolve כתובות של ישויות חיצוניות, merge של object files, עטיפה ב-**ELF**; **undefined reference** מגיע מה-linker; **`_start`** חלק מ-**CRT** וקורא ל-`main()`; `printf` ב-`libc.so`
- [ ] מבנה **ELF** — פורמט \*nix (לא Mach-O, לא **PE**); סקשנים: **`.text`**, **`.data`**, **`.bss`**, **`.rodata`**, **`.interp`**, **`.symtab`+`.strtab`**
- [ ] **10 שלבים** להרצת executable — `execve` (syscall 59), `open()`+קריאת ELF header, אימות **magic** (**`\x7FELF`**), program header, `mmap()` (file-backed), יצירת PCB, הקצאת stack (`argc`/`argv`/`envp`), יצירת thread עם **RIP**=`e_entry`, חזרה מ-syscall
- [ ] מבנה ה-**stack** בהתחלה (`argc`/`argv`/`envp`/`auxv`), **RBP** נקבע רק ב-prologue; חזרה על **function prologue/epilogue**
- [ ] **תרגול:** ניתוח **ELF header** ב-hex — magic `7f454c46`, סוג (`02 00`), ארכיטקטורה x86_64 (`3e 00`), **entrypoint** ב-little-endian; `.text` (r/x) מול `.data` (r/w)
- [ ] למה קשה להריץ executable בין מערכות הפעלה; **WINE** טוען **PE** נייטיב ב-Linux
- [ ] **static library** — ארכיון object files (`ar rcs`), מקושר ב-link-time והופך חלק מה-executable; **`.lib`** / **`.a`**
- [ ] **dynamic library** — נטען ב-runtime, ה-VM שלו **משותף** (נטען פעם אחת) → פחות זיכרון, מודולריות, hot patching; **`.dll`** / **`.dylib`** / **`.so`**; static vs dynamic **linking** (טעינה ב-startup מול **`LoadLibrary`/`dlopen`** + **`GetProcAddress`/`dlsym`**)
- [ ] **10 שלבים** לטעינת **`.so`** — `dlopen()` (ללא syscall ייעודי), search order, `open()`+ELF header, אימות magic, program header, `mmap()`+הרשאות, **relocation**, אתחול **symbol table**, קוד אתחול, החזרת **handle**
- [ ] **Windows `LoadLibrary` search order** (ההתאמה הראשונה) — נתיב, תיקיית exe, **`system32`**/**`SysWOW64`**, current dir, `PATH`; סכנות: DLL זדוני, גרסאות מרובות
- [ ] **Export Table** (Windows) — **Ordinal**/**Name**/**RVA**; **`__declspec(dllexport)`**, **name mangling** ב-C++ מול **`extern "C"`**; **`GetProcAddress`** פורס את ה-PE (DOS→NT headers→export dir→`base+RVA`)
- [ ] **static link** ל-dynamic library — **import library** ב-Windows: **`__declspec(dllimport)`**, `.lib` עם קריאה עקיפה דרך **Import Table** / **PE IAT**, dependency tree
- [ ] הרצת DLL מקושר סטטית — **`CreateProcess`** → **Windows PE loader** (מעבר על import table, טעינת DLLs, עדכון **IAT**, relocation; כישלון=**SEH**) → `_start` → CRT → `main`
- [ ] **relocation** ב-DLL — קריאה פנימית = **relative jump**, אך משתנים גלובליים דורשים **absolute**; **preferred base address**; טעינה לבסיס שונה → תיקון; מבצע: **Windows PE loader**
- [ ] **תרגול:** טבלת **`.reloc`** — דפי 4KB, offset+type; חישוב **delta** (`0x10000000`→`0x15000000`, Δ=`0x5000000`, `mov rax,0x10002000`→`0x15002000`)
- [ ] **copy-on-write (COW)** — write לדף משותף → עותק פרטי; OS מסמן read-only, MMU מזהה write → page fault → דף פרטי (גם ב-`fork`); ההקשר: שיתוף דפי DLL
- [ ] **תרגול:** שלוש דוגמאות **COW** מפורטות — preferred מול non-preferred, מתי relocation/COW/דף חדש; דוגמה 3: shared/private + הרשאות + סוג דף (**anonymous**/**file-backed**) עבור `int myX`
- [ ] **lazy vs eager binding** — eager: פתרון סימבולים בטעינה; lazy: בקריאה הראשונה; **Windows eager** / **Linux lazy** כברירת מחדל
- [ ] **DLL entrypoint / `DllMain`** — `DLL_PROCESS_ATTACH`/`DETACH`, `DLL_THREAD_ATTACH`/`DETACH`; מונה LoadLibrary/FreeLibrary
- [ ] **DLL Hell** — תלות בגרסאות שונות של אותו DLL; **WinSxS** (Side-By-Side) ממתן; Linux נמנע מנתיבי חיפוש גלובליים קשיחים
- [ ] **`.so` ב-\*nix** + `dlopen` **search order** — **`RPATH`/`RUNPATH`** ב-ELF; סדר: `DT_RPATH` → `LD_LIBRARY_PATH` → `DT_RUNPATH` → `/etc/ld.so.conf` → `/lib` → `/usr/lib` (**לא** current dir)
- [ ] **symbol resolution** + **PLT** + **GOT** — הטמעה ב-**GOT** ולא נפתר שוב; **PLT** ב-`.text` (stub/trampoline לכל קריאה חיצונית); **GOT** ב-`.data` (r/w, כתובות שנפתרו, דפיו הופכים private ב-**COW** בעת patching)
- [ ] **תרגול:** **PLT+GOT** קריאה ראשונה (lazy) מול מאוחרת — stub בן 4 הוראות: `jmp [got]` → `pushq <index>` → `jmp _dl_runtime_resolve` (relocation דרך `.rela.plt`, מעדכן GOT) → קריאה מאוחרת קופצת ישירות מה-GOT
- [ ] **`dlopen` flags** + visibility — **`RTLD_LOCAL`**/**`RTLD_GLOBAL`**, **`RTLD_LAZY`**/**`RTLD_NOW`**; `RTLD_DEFAULT`/`RTLD_NEXT`; טבלת **`.dynsym`**
- [ ] **symbol visibility** — `default` / `hidden` / `protected` / `internal`; `-fvisibility`, `__attribute__((visibility))`, `-Bsymbolic`; דוגמאות דריסה
- [ ] **import library ב-\*nix** — אין `.lib` נפרד; מידע הקישור ב-ELF עצמו (**`.dynamic`**, רשומת **NEEDED**), הסימבול נשאר **UND** ונפתר ב-runtime
- [ ] **`DT_INIT`/`DT_FINI`** — `__attribute__((constructor/destructor))`; legacy (מצביע יחיד) מול modern (מערך); חתימה `void(void)`
- [ ] **PIC (Position Independent Code)** — `-fPIC`: כתובות יחסיות; בלי PIC — **absolute** ויותר relocations; executables מעדיפים absolute (נטענים בכתובת קבועה)

### יחידה 21 — וירטואליזציה (Virtualization)  ·  *(Rec7 בלבד)*  🔻 *נושא בונוס — כנראה לא במבחן; ללמוד אחרון (ראו "מחוץ לחומר")*
- [ ] רקע והגדרה — **virtualization** מבודד מערכת הפעלה שלמה (**guest**) מעל מכונה פיזית (**host**), מנוהל ע"י **hypervisor / VMM**
- [ ] **hypervisor type 1** (bare-metal — ESXi/Hyper-V/Xen/**KVM**, overhead נמוך) מול **type 2** (hosted — VirtualBox/VMware Workstation, overhead גבוה יותר); **KVM** = מודול קרנל שהופך את Linux ל-type 1
- [ ] **CPU virtualization** — **privileged instruction** (עושה trap מחוץ ל-kernel mode) מול **sensitive instruction** (התנהגות תלוית privilege); בעיית x86 (~17 הוראות sensitive-not-privileged)
- [ ] שיטות — **trap-and-emulate**, **full emulation** (QEMU), **binary translation** (VMware), **paravirtualization** (Xen + **hypercalls**), **system call translation** (WSL1)
- [ ] **hardware-assisted** (VT-x/AMD-V, Ring -1/VMX root, **VM exit**/entry — יקר), WSL2; ספקטרום overhead: full emulation → binary translation → paravirt → hardware-assisted
- [ ] **memory virtualization** — תרגום כפול **GVA→GPA→HPA**; **shadow page tables** מול **EPT/NPT** (חומרה, 2D page walk); tagged TLB (**VPID**/**ASID**)
- [ ] **container vs VM** — container חולק את kernel המארח (**namespaces** + **cgroups**), VM נושא guest OS מלא; **Docker** (image↔container, layers, Dockerfile); K8s
- [ ] אבטחה — **VM escape**, containers חלשים יותר (kernel משותף); הגנות: seccomp, AppArmor/SELinux, rootless, **microVM** (Firecracker); **cloud abstraction ladder** — Physical → **IaaS** → **CaaS** → **FaaS**

---

## 🧩 מיומנויות רוחביות (נבדקות לרוחב הקורס)

> הקורס בוחן דרך **חישוב ידני**. ודאו שאתם שולטים במיומנויות שחוצות מספר יחידות:

- [ ] **חישובי זמן ותזמון:** utilization/overhead/usage/wait/latency/throughput מ-time budget; **Gantt charts** + זמן המתנה/turnaround ממוצע ל-FCFS/SJF/RR/MLFQ/RSDL, כולל I/O bursts
- [ ] **תרגום כתובות:** VA→PA דרך **multi-level page table** ביד; VPN/offset; **TLB hit / TLB miss / page fault** — מה עושה ה-MMU ומה עושה ה-OS בכל אחד
- [ ] **החלפת דפים:** הרצת **FIFO/LRU/Clock** על reference string וספירת faults; הדגמת **Belady**; חישוב **EAT**
- [ ] **מערכת קבצים:** גודל קובץ מקסימלי / overhead של indirect blocks ב-inode; גודל טבלת FAT; מספר I/O לאיתור קובץ; **קיבולת RAID ושחזור XOR / weighted-XOR**
- [ ] **מקביליות:** אנומרציית **interleavings** של threads וכל הערכים הסופיים האפשריים; זיהוי **race / deadlock / starvation** ותיקונם; הוכחות על מנעולים (FIFO, bounded waiting)
- [ ] **אריתמטיקת base-2 תמיד** (1KB=1024B) — המבחן קובע זאת מפורשות
- [ ] **קריאה/מעקב קוד:** C, x86/x64 assembly, ותיקון באגים בקוד סנכרון/signals/uthreads; ניתוח "מה ה-output?"
- [ ] הבחנות מושגיות חוזרות: **trap / interrupt / exception**; **mechanism vs policy**; **kernel mode vs user mode**; **concurrency vs parallelism**

---

## 📝 תרגול מבחן — לפי מבחן התרגול (`Exams-md/Practice-Exam.md`)

> מבנה: חלק א׳ (10 נכון/לא-נכון), חלק ב׳ (4 אמריקאיות), חלק ג׳ (עונים על 2 מתוך 3 פתוחות). פתרו **closed-book** ואז השוו לפתרון המלא (כולל פירוק הניקוד לתת-סעיפים).

**חלק א׳ — נכון/לא-נכון (עברו על כל 10):**
- [ ] **A1** TAS spinlock **אינו** מבטיח שכל thread ממתין ישיג את המנעול (לא starvation-free) · **A2** ערכי החזרה של `setjmp`/`longjmp` · **A3** RR עם quantum גדול מכל burst ≡ **FCFS**
- [ ] **A4** האם multi-level page table תמיד צורך יותר זיכרון (לא — מרחב דליל חוסך) · **A5** RAID 5 שורד כשל דיסק **אחד** בלבד · **A6** lazy binding נפתר בקריאה הראשונה (לא בטעינה)
- [ ] **A7** `cond_wait` — למה חובה `while` ולא `if` (re-check הפרדיקט) · **A8** divide-by-zero (**exception**) מול `write()` (**system call**) · **A9** האם context switch מחייב תמיד ניקוי TLB (לא — PCID/ASID) · **A10** נקודת הכניסה היא **`_start`** ולא `main`

**חלק ב׳ — אמריקאיות (עברו על כל 4):**
- [ ] **B1** `setjmp` ואז הפונקציה **חוזרת**, ואז `longjmp` → **undefined behavior** (קפיצה ל-stack frame מת)
- [ ] **B2** מעריך burst `τₙ₊₁ = α·tₙ + (1−α)·τₙ` (α=0.5, τₙ=8, tₙ=4 → **6**)
- [ ] **B3** חישוב overhead של **indirect blocks** ל-inode (קובץ 100MB, 12 direct + single + double + triple, block 4KB → **104KB**)
- [ ] **B4** מה ה-**GOT entry** מצביע אליו לפני הקריאה הראשונה (חזרה ל-PLT stub / ה-resolver)

**חלק ג׳ — פתוחות (בחרו 2 מתוך 3):**
- [ ] **C1** interleaving של threads תחת mutex — כל הערכים הסופיים האפשריים של `x`, סדר לערך המינימלי, **deadlock** עם שני mutexes וכלל ה-**lock ordering**, ואנומרציה עם thread שלישי
- [ ] **C2** **copy-on-write** עם threads ו-`fork()` — מילוי טבלאות (Fault? / Copy? / מצב הדף) ל-threads שחולקים page table, ואחרי `fork()` בהורה ובילד (כולל ה-discriminator: כתיבה שנייה של ההורה ללא copy)
- [ ] **C3** **MLFQ מול RSDL** — סדר סיום תחת כל אחד, וטבלת **Gantt** לכל ms; שים לב ל-**alter question** (ה-MLFQ כאן משאיר thread חוסם באותה רמה, בניגוד לגרסת הכיתה שמקדמת)

---

## ⛔ מחוץ לחומר הבחינה

> מסומן כאן רק כדי שהצ׳קליסט יהיה מלא ולא "ישכח" נושא.

- **וירטואליזציה — יחידה 21 (Rec7).** לפי המתרגל בהקלטת התרגול האחרון: *"בשבוע הבא… נדבר על איזשהו נושא בונוס כזה שלא יהיה לבחינה"* — והתרגול הבא הוא בדיוק וירטואליזציה. גם בהרצאה האחרונה נאמר *"זה כל מה שיהיה למבחן"*. הראיה חזקה אך עקיפה (הערה מדוברת בהקלטה רועשת) — **אשרו מול צוות הקורס** לפני שמוותרים לגמרי. נשאר בצ׳קליסט מסומן 🔻 כדי ללמוד אותו אחרון או לדלג אם הזמן קצר.
- **החלק המנהלי** בתחילת התרגולים/ההרצאות (מתרגלים, מבנה HW, Moodle) — לא חומר לימוד.

---

*נבנה מסריקה מלאה של 7 ההרצאות, 7 התרגולים, 3 מערכי פתרונות התרגילים ומבחן התרגול. אם נוסף חומר או השתנה היקף המבחן — עדכנו את הרשימה בהתאם. שימו לב לאסימטריות בין המסלולים (ראו הטבלה בראש הקובץ) — איחוד שני המסלולים הוא הסילבוס.*
