/* PGO training run and result check for the JNI library (build_dll.bat).
 *
 *   java pgo\GCalBench.java <path to gcal-11j.dll> [repeats]
 *   java pgo\GCalBench.java <path to gcal-11j.dll> 1 verify
 *
 * The first form runs the training workload through the only JNI entry point,
 * Java_org_gaurabda_GCalManager_nativeGCal4Query: one month of q=calendar,
 * q=sankranti, q=naksatra, q=tithi, q=firstday, q=masastart, q=paksastart,
 * q=gtithi, q=next and q=gnaksatra for four locations (both hemispheres, both
 * sides of Greenwich, with and without DST) and six years from 1950 to 2099.
 * Prints the best time of the repeats, an FNV-1a 64-bit checksum over the
 * UTF-8 bytes of every response and the largest response. Two DLLs with the
 * same checksum give the same XML, byte for byte, on this workload.
 *
 * The second form runs a wider workload (every year 1900-2100 for the yearly
 * queries, other dates and counts), not timed and not used for training, and
 * prints only its checksum.
 *
 * Every response must stay under the 64 KB of the library's output buffer
 * (XMLOut in astro_w.cpp, a deliberate limit): q=calendar is asked one month at
 * a time, q=gcalendar (a whole Gaurabda year) is not asked; nor is q=appday,
 * which returns the buffer of a TString already destroyed.
 *
 * The class is org.gaurabda.GCalManager, so that the JNI name of the library
 * resolves. Run in source-file mode (JDK 11+), nothing to compile. ASCII only.
 */
package org.gaurabda;

import java.nio.charset.StandardCharsets;

public class GCalManager {
    static native String nativeGCal4Query(String gcalQuery);

    static final String[] LOCATIONS = {
            "&lc=Kyiv&la=50.4501&lo=30.5234&lt=2E00&dst=3x0x5x0x10x0x5x0",
            "&lc=Mayapur&la=23N25&lo=88E24&lt=5E30&dst=0x0x0x0x0x0x0x0",
            "&lc=New+York&la=40N43&lo=74W00&lt=5W00&dst=3x0x2x0x11x0x1x0",
            "&lc=Sydney&la=33S52&lo=151E12&lt=10E00&dst=10x0x1x0x4x0x1x0",
    };
    static final int[] YEARS = {1950, 1980, 2000, 2024, 2050, 2099};
    static final int[] MONTH_DAYS = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    static long h;
    static int maxLen;
    static int calls;

    static void mixByte(int b) {
        h ^= b & 0xff;
        h *= 1099511628211L;
    }

    static void query(String q) {
        String r = nativeGCal4Query(q);
        calls++;
        if (r == null) {
            mixByte(0xff);
            return;
        }
        byte[] b = r.getBytes(StandardCharsets.UTF_8);
        if (b.length > maxLen) maxLen = b.length;
        for (byte x : b) mixByte(x);
        mixByte(0);
    }

    static int monthDays(int y, int m) {
        boolean leap = (y % 4 == 0 && y % 100 != 0) || y % 400 == 0;
        return m == 2 && leap ? 29 : MONTH_DAYS[m - 1];
    }

    static void year(String loc, int y, int m) {
        query("q=calendar" + loc + "&ty=" + y + "&tm=" + m + "&td=1&tc=" + monthDays(y, m));
        query("q=sankranti" + loc + "&ty=" + y + "&tm=1&td=1&tc=370");
        query("q=naksatra" + loc + "&ty=" + y + "&tm=3&td=1&tc=30");
        query("q=tithi" + loc + "&ty=" + y + "&tm=6&td=15&th=6&tmin=30");
        query("q=firstday" + loc + "&ty=" + y);
        query("q=masastart" + loc + "&ty=" + y + "&tm=9&td=10");
        query("q=paksastart" + loc + "&ty=" + y + "&tm=9&td=10");
        query("q=gtithi" + loc + "&ty=" + y + "&tye=" + (y + 1) + "&gm=7&gp=1&gt=15");
        query("q=next" + loc + "&ty=" + y + "&tm=1&td=1&gm=3&gp=0&gt=11");
        query("q=gnaksatra" + loc + "&gy=" + (y - 1486) + "&gm=5&tc=10");
    }

    static void train() {
        // one month of q=calendar per location and year (74 ms each - it is the slowest
        // query; together with q=gtithi most of the time): over the 24 location-years
        // every month of the year comes twice
        int i = 0;
        for (String loc : LOCATIONS)
            for (int y : YEARS) year(loc, y, 1 + (i++ * 5) % 12);
    }

    static void verify() {
        for (String loc : LOCATIONS) {
            for (int y = 1900; y <= 2100; y += 1) {
                query("q=sankranti" + loc + "&ty=" + y + "&tm=1&td=1&tc=366");
                query("q=firstday" + loc + "&ty=" + y);
                query("q=calendar" + loc + "&ty=" + y + "&tm=" + (1 + y % 12) + "&td=" + (1 + y % 28) + "&tc=7");
                for (int san = 0; san <= 3; san++)
                    query("q=calendar" + loc + "&ty=" + y + "&tm=4&td=10&tc=7&san=" + san);
            }
            for (int y = 1800; y <= 2400; y += 25) {
                query("q=tithi" + loc + "&ty=" + y + "&tm=" + (1 + y % 12) + "&td=20&th=23&tmin=59");
                query("q=naksatra" + loc + "&ty=" + y + "&tm=1&td=1&tc=60");
                query("q=masastart" + loc + "&ty=" + y + "&tm=3&td=1");
                query("q=paksastart" + loc + "&ty=" + y + "&tm=12&td=31");
                for (int gm = 0; gm < 13; gm++)
                    query("q=next" + loc + "&ty=" + y + "&tm=1&td=1&gm=" + gm + "&gp=" + (gm & 1) + "&gt=" + (1 + gm));
            }
        }
    }

    public static void main(String[] args) {
        System.load(new java.io.File(args[0]).getAbsolutePath());
        int reps = args.length > 1 ? Integer.parseInt(args[1]) : 5;
        if (args.length > 2 && args[2].equals("verify")) {
            h = 0xcbf29ce484222325L;
            verify();
            System.out.printf("verify %016x calls %d max %d%n", h, calls, maxLen);
            return;
        }
        long best = Long.MAX_VALUE;
        long sum = 0;
        for (int r = 0; r < reps; r++) {
            h = 0xcbf29ce484222325L;
            calls = 0;
            long t0 = System.nanoTime();
            train();
            long t = System.nanoTime() - t0;
            if (t < best) best = t;
            if (r > 0 && h != sum) throw new IllegalStateException("checksum differs between repeats");
            sum = h;
        }
        System.out.printf(java.util.Locale.ROOT, "train %016x calls %d max %d best %.3f s%n",
                sum, calls, maxLen, best / 1e9);
    }
}
