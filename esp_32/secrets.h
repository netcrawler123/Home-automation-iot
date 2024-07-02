#include <pgmspace.h>
 
#define SECRET
#define THINGNAME "home_auto"                         //change this
 
const char WIFI_SSID[] = "realme";               //change this
const char WIFI_PASSWORD[] = "12345678";           //change this
const char AWS_IOT_ENDPOINT[] = "aal0jkuij6i6u-ats.iot.ap-south-1.amazonaws.com";       //change this
 
// Amazon Root CA 1
static const char AWS_CERT_CA[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbSNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";
 
// Device Certificate                                               //change this
static const char AWS_CERT_CRT[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
MIIDWTCCAkGgAwIBAgIUHSLOrrPZfHTxlBvsnFzCkhKeSN8wDQYJKoZIhvcNAQEL
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTIzMTAxMjE0MzY0
MVoXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0
ZTCCASIwDQYJKoZIhvcNAQEBBQADg9PIZf0UvAE12IyJed4x0
od6FMlP00D4a/Vo/nLTjNR061Bw3a5O97QZ6uh0z2smdRX2rQTo8eICjDMR86I/3
YkAfgANgoAJjbsYrm0jJzUmM7fNFIJV5j0J/Z+hhW8l6pKHeRP0JeEHq4tdsg1CU
y3ZKhW62V2c/eTEwWnZ/Prysg+qPbVepKkn1nD2ZbIxJ4mETmAkzQqN8v2TBJGAY
k0lBV0txXnZ46uCo2pTa93VQIh7IQv29FFlTSpeX8qbCmPIVKFEam/PwdqxBFYxa
jgBdxAiasNAOODTQ7ZGkcB6dgxu95I5d1R7Bv+uPdZ6IwaZTiuUb6fjLtap6Pujn
5tH+0AM2zZd6TsIOE6JqueMNmQpVrskdd9OMRQh1Hb0MLmZ5J7SviAmRg+HYgAMt
klWJrEN+PcxWzzO1ym+jBtLHpEVfLwPFmlxROED3mY52l1ab9Jse7Ot2BKhZ
-----END CERTIFICATE-----

 
 
)KEY";
 
// Device Private Key                                               //change this
static const char AWS_CERT_PRIVATE[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
MIIEpAIBAAKCAQEAt4DbQVSi2Aygxz7v8+5BRwscOd80xS8Kg0754MpOjkdwzeur
d3oMeh8Nht0UEDf08hl/RS8ATXYjIl53jHSh3oUyU/TQPhr9Wj+ctOM1HTrUHDdr
k73tBnq6HTPayZ1FfatBOjx4gKMMxHzoj/diQB+AA2CgAmNuxiubSMnNSYzt80Ug
lXmPQn9n6GFbyXqkod5E/Ql4Qeri12yDUJTLdkqFbrZXZz95MTBadn8+vKyD6o9t
V6kqSfWcPZlsjEniYROYCTNCo3y/ZMEkYBgWduLHGWbO88RcopTvM4/Gqd1+Umn3
oCaIdoUYWL9P5W/YW9exu+vZ5OBiDOgqqVTBU75sFjBuBvoN4jVsvgLZ0XUrM3R
0vR5z5X6UNKEHDAyzstQUPuivr9Eqx53n6O+is
SVk9bIVGUmPk8PirMQHuRbXJ5D9tASqlvGcz5poIbable5mxvcQzGiD1N77FbJF6
8kRAzSgv0OlhZCGMZznAFHrEvlo4j/zyemmdxSJ70IB9VEe91xTM6egSOBROlLH2
UY5pmyal6YUhG5nkLR51i1upnDq9cSEgf2pbUmx6flIsg1wZSuhl+MuV9ixxiejy
P+TOe0ECgYEA4q+GUh4zXwEyGnOjamkcG/tGKqQVsoNNur/GCpoVj55ra2/afsVO
1ZAQLtkTZL7yJj1FfUFdjsECgYAtuhW87U5aeIrOltesyXaFEZENrnZGpC8TBpsa
xYz5EGIfhVmqo83/23fsP6i5wfHCUtpdHEqQAuGVvBXA6qqkjorCZt0n7lfeix9z
HJ7vBKbptq6gQGblvh84zQ3S4lGW/RTb6UwJMBqBGb8unO4AkZAKunxPuzEr+usn
pvuzAwKBgQDKHy1mbrbij/z6Ixdlc1sMFm7piPVWcuwE5KIP8pdKmi0UFyuRarZy
+xwMyEMYC54ex3MoTkQVv8RTRwS4/5o3lt1oPeiMfYsTqe6oW085BBdnlV5izyRH
SrXOz/ZnPtpOSIqI+BuEelD2ggMyVT5DEt1ycS2amm6DKttQXNQepw==
-----END RSA PRIVATE KEY----- 
)KEY";