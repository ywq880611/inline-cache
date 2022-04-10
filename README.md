It's a simple implement of inline cache.
I got this idea from [here](https://mrale.ph/blog/2012/06/03/explaining-js-vms-in-js-inline-caches.html).

Now I just finished property(Named), here are results tested in my mac(x64):


```IC: 0.276818 seconds```

```No IC: 2.248275 seconds```

I am going to implement elements(Keyed) IC.