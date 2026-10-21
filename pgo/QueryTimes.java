/* Time per query kind (one location, one year): which queries dominate GCalBench.
 *   java pgo\QueryTimes.java <path to gcal-11j.dll>
 * ASCII only. */
package org.gaurabda;

public class GCalManager {
    static native String nativeGCal4Query(String gcalQuery);

    public static void main(String[] args) {
        System.load(new java.io.File(args[0]).getAbsolutePath());
        String loc = "&lc=Kyiv&la=50.4501&lo=30.5234&lt=2E00&dst=3x0x5x0x10x0x5x0";
        int y = 2024;
        String[] qs = {
                "q=calendar" + loc + "&ty=" + y + "&tm=1&td=1&tc=31",
                "q=sankranti" + loc + "&ty=" + y + "&tm=1&td=1&tc=370",
                "q=naksatra" + loc + "&ty=" + y + "&tm=3&td=1&tc=30",
                "q=tithi" + loc + "&ty=" + y + "&tm=6&td=15&th=6&tmin=30",
                "q=firstday" + loc + "&ty=" + y,
                "q=masastart" + loc + "&ty=" + y + "&tm=9&td=10",
                "q=paksastart" + loc + "&ty=" + y + "&tm=9&td=10",
                "q=gtithi" + loc + "&ty=" + y + "&tye=" + (y + 1) + "&gm=7&gp=1&gt=15",
                "q=next" + loc + "&ty=" + y + "&tm=1&td=1&gm=3&gp=0&gt=11",
                "q=gnaksatra" + loc + "&gy=" + (y - 1486) + "&gm=5&tc=10",
        };
        for (String q : qs) {
            long t0 = System.nanoTime();
            String r = nativeGCal4Query(q);
            long t = System.nanoTime() - t0;
            System.out.printf(java.util.Locale.ROOT, "%8.1f ms %6d  %s%n", t / 1e6, r == null ? -1 : r.length(), q.substring(0, q.indexOf('&')));
        }
    }
}
