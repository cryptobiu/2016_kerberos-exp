
This utility was built to simple ask the KDC for new tickets,
simulating a number `n' of clients performing the request in
parallel.

The amount of clients and threads can be specified by a
command line option, the same for other parameters such as:
username and password. The complete list of parameters can
be found below:

```
	-r n	Amount of requests
	-c n	Amount of parallel clients
	-u	User name (e.g. user@KRB.ZIMMERLE.ORG)
	-t	TGT name (e.g. krbtgt/KRB.ZIMMERLE.ORG@KRB.ZIMMERLE.ORG)
	-s	Service (e.g. HTTP/www.example.com@KRB.ZIMMERLE.ORG)
	-p	Password
```

**Notice:** this small software was built using the default
Kerberos token generation, further it will be changed to use
Secure Computation.

# How to build

## Depdencies

Make sure you have installed on your system the krb5 development
package or have the MIT kerberos installed from source.

## Compilation

This utility compilation can be performed by the utilization of
the command make. No fancy autotools setup was built, since we might
not need it. If compilation becomes a problem, time will be spent on it.

# Items for improvments

There are some missing features on the utility, including, but not
limited to:

 - [ ] Output in a CSV format, to be easy imported in any other software.
 - [ ] Option to set a delay between the token requests.


