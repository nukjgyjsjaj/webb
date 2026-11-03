# الاختبارات الخمسة (T1..T5)

T1 slot_replay: يعيد تشغيل slot مسجّل بترتيبين (arrival vs indexed+write_version)،어야 تكون النتيجة متطابقة لكل بركة.
T2 quote_diff: آلاف الأحجام العشوائية لكل DEX، مقارنة مع simulateTransaction — فرق صفر في وحدات التوكن (عدا تقريب الجذر).
T3 bank_fork: تحديثان لنفس المفتاح على أبوين مختلفين — الصفقة تنجو فقط لبنك Processed ثم Rooted.
T4 compute_latency: p50/p99 من اكتمال فك الشرد حتى قرار الإرسال — p99 < 1ms.
T5 paper_vs_landed: إذا كان الورقي موجباً والهبوط سالباً، النموذج مرفوع.

نتائج موثقة في REPORT.md.
