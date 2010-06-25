/*
 * Copyright (c) 2010, Andras Varga and Opensim Ltd.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Opensim Ltd. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Andras Varga or Opensim Ltd. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

package org.omnetpp.scave.writers.impl;

import java.io.PrintStream;
import java.lang.management.ManagementFactory;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Map;

/**
 * Common base class for FileOutputVectorManager and FileOutputScalarManager
 *
 * @author Andras
 */
public abstract class OutputFileManager  {

    public static String generateRunID(String baseString) {
        String dateTime = new SimpleDateFormat("yyyyMMdd-HH:mm:ss").format(new Date());
        String pid = ManagementFactory.getRuntimeMXBean().getName();
        return baseString + "-" + dateTime + "-" + pid;
    }

    protected void writeRunHeader(PrintStream out, String runID, Map<String, String> runAttributes) {
        out.println("run " + q(runID));
        writeAttributes(out, runAttributes);
        out.println();
    }

    protected void writeAttributes(PrintStream out, Map<String, String> attributes) {
        if (attributes != null)
            for (String attr : attributes.keySet())
                out.println("attr " + q(attr) + " " + q(attributes.get(attr)));
    }

    /**
     * Quotes the given string if needed.
     */
    protected static String q(String s) {
        if (s == null || s.length() == 0)
            return "\"\"";

        boolean needsQuotes = false;
        for (int i=0; i<s.length(); i++) {
            char ch = s.charAt(i);
            if (ch == '\\' || ch == '"' || Character.isWhitespace(ch) || Character.isISOControl(ch)) {
                needsQuotes = true;
                break;
            }
        }

        if (needsQuotes) {
            StringBuilder buf = new StringBuilder();
            buf.append('"');
            for (int i=0; i<s.length(); i++) {
                char ch = s.charAt(i);
                switch (ch)
                {
                    case '\b': buf.append("\\b"); break;
                    case '\f': buf.append("\\f"); break;
                    case '\n': buf.append("\\n"); break;
                    case '\r': buf.append("\\r"); break;
                    case '\t': buf.append("\\t"); break;
                    case '"':  buf.append("\\\""); break;
                    case '\\': buf.append("\\\\"); break;
                    default:
                        if (!Character.isISOControl(ch))
                            buf.append(ch);
                        else
                            buf.append(String.format("\\x%02.2x", ch));
                }
            }
            buf.append('"');
            s = buf.toString();
        }
        return s;
    }
}
