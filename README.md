### Emulating spontaneous cortical activity seen in-vivo through ![RTXI](https://github.com/RTXI/rtxi)

**Requirements:** None  
**Limitations:** None  



<!--start-->

This module aims at reproducing the synaptic bombardment targetting a single cell in cortical networks. The currents are based on Poisson point processes convoluted with exponential waveforms. For the rationale of such models and similar dynamic-clamp implementation (for in-vitro experiments), see e.g. the (non-exhaustive) following publications :

#### N.B. This classification has not been peer-reviewed and does not
     constitute a "state-of-the-art" paradigm! The three proposed
     regimes are only a vague scheme of what can be observed in the
     "in vivo" cortex. The theoretical model underlying the ongoing
     dynamics has been derived through arbitrarily-taken network
     activity waveforms.

![Deep Anesthesia](in-vivo-like-cortical-act-deep-anesth.png)

![Sleep-like Activity](in-vivo-like-cortical-act-sleep.png)

![Awake-like Activity](in-vivo-like-cortical-act-awake.png)

The config file producing the snapshot (i.e. combined with my LIF-model implementation) can be found on this ![link](https://github.com/yzerlaut/my_rtxi_configs/blob/master/lif-shotnoise-input.set)


<!--end-->

####Parameters

hidden in the code

Shift Cortical State through the GUI button !


