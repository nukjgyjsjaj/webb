# VLESS Proxy Sources

## Active Sources (Updated Every 15 Minutes)

1. **vless-proxy-list** (Every 4 hours)
   - URL: https://raw.githubusercontent.com/26info/vless-proxy-list/main/working-proxies.txt
   - Format: vless:// links

2. **Proxy-List** (Every hour)
   - URL: https://raw.githubusercontent.com/Proxy-List/Proxy-List/main/vless.txt
   - Format: vless:// links

3. **FreeProxyList** (Every 10 minutes)
   - URL: https://raw.githubusercontent.com/mahdibland/V2RayAggregator/master/sub/vmess.txt
   - Format: vless:// and vmess:// links

4. **Vless Collector** (Continuous)
   - URL: https://raw.githubusercontent.com/VlessCollector/working-proxies/main/vless.txt
   - Format: vless:// links

5. **Freedom-V2Ray** (Every 2 hours)
   - URL: https://raw.githubusercontent.com/freedom2023-fyp/V2ray/main/vmess.txt
   - Format: vless://, vmess://, trojan://, reality:// links

## Link Format
```
vless://uuid@server:port?type=ws&encryption=none&security=tls&path=%2fcdn-cgi%2fws#name
```

## Parsing
The VLESS Collector parses these links to extract:
- UUID (user identifier)
- Server address and port
- Transport type (ws, tcp, reality)
- Security settings (tls, reality, none)
- Path/flow parameters

## Validation
Links are tested automatically:
1. Connection test (5 second timeout)
2. Page load test (google.com)
3. Speed test (latency check)
4. Stability test (multiple requests)

Invalid/slow links are automatically removed from the active list.

## Fallback
If all online sources fail, the browser falls back to:
- Local cached list (valid for 24 hours)
- Embedded hardcoded fallback links (emergency only)

## Security
- Links are never stored to disk permanently
- Links are deleted from memory on session end
- All links are validated before use
- No link reuse between sessions
