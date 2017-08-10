/*
 * Copyright (C) 2011 Georgia Institute of Technology, University of Utah,
 * Weill Cornell Medical College
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * This is a template implementation file for a user module derived from
 * DefaultGUIModel with a custom GUI.
 */

#include "in-vivo-like-cortical-act.h"
#include <iostream>
#include <main_window.h>

extern "C" Plugin::Object*
createRTXIPlugin(void)
{
  return new InVivoLikeCorticalAct();
}

double InVivoLikeCorticalAct::RdmNumber() {
  double rdm_number;
  /* (nastyly generated) uniformily distributed random number */  
  rdm_number = (rand() % 1000000) * 1e-6 ; 
  return rdm_number;
}

void InVivoLikeCorticalAct::conductance_update(double* Ga, double* Ge, double* Gi) 
{
  *Ga *= exp(-period/steps/(1e-3*Te)) ; 
  *Ge *= exp(-period/steps/(1e-3*Te)) ; 
  *Gi *= exp(-period/steps/(1e-3*Ti)) ; 
  if (RdmNumber()<Na*Fa*period/steps) { *Ga += Qa;} // kept in nS here
  if (RdmNumber()<Ne*Fe*period/steps) { *Ge += Qe;} // kept in nS here
  if (RdmNumber()<Ni*Fi*period/steps) { *Gi += Qi;}
}

void InVivoLikeCorticalAct::Convert_Fa_to_Shotnoise_Freqs() {
  double x;
  x = log(Fa+1e-5);
  Fe = exp( -56.550681238*pow(x,12) +
	    1496.63903733*pow(x, 11)
	    -17952.4658914*pow(x,10)
	    +128992.047099*pow(x,9)
	    -617999.512402*pow(x,8)
	    +2078750.16648*pow(x,7)
	    -5031200.42987*pow(x,6)
	    +8824045.46324*pow(x,5)
	    -11125673.8299*pow(x,4)
	    +9831154.25688*pow(x,3)
	    -5777670.27308*pow(x,2)
	    +2027265.10702*pow(x,1)
	    -321162.542745);

  Fi = exp(-35.0352278809*pow(x,12)
	   +933.942765539*pow(x,11)
	   -11283.6946933*pow(x,10)
	   +81656.9023314*pow(x,9)
	   -393994.162019*pow(x,8)
	   +1334548.73485*pow(x,7)
	   -3252242.27083*pow(x,6)
	   +5742448.17542*pow(x,5)
	   -7287927.18875*pow(x,4)
	   +6481175.90263*pow(x,3)
	   -3832586.64742*pow(x,2)
	   +1352868.85255*pow(x,1)
	   -215571.343599);
    }
    
/* We need a sign function, taken from 
https://stackoverflow.com/questions/1903954/is-there-a-standard-sign-function-signum-sgn-in-c-c 
*/  
template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}


void InVivoLikeCorticalAct::Temporal_Dynamics_of_Freqs() {
  /* 
     Arbitrary functions for the temporal dynamics of Fa
     producing the three desired cortical states
  */
  double y0;
  if (state_value==0) {
       y0 = pow(cos(2.*M_PI*sin(3.*systime)), 6);
       Fa = 13.*(sgn(y0-0.8)+1)*y0/4.;
       Convert_Fa_to_Shotnoise_Freqs() ;
       Fe = 2*Fe+1e-2;
       Fi /= 2.;
       Fa /= 2.;
    }
  else if (state_value==1) {
      Fa = 10.*(1+sgn(cos(2.*M_PI*sin(2.5*systime)))) ;
      Convert_Fa_to_Shotnoise_Freqs() ;
      Fe += 4e-2;
      Fi += 2e-2;
    }
  else if (state_value==2) {
    Fa = 3.*(1+(sin(2.*M_PI*sin(0.7*systime))+1)+
	     1/2.*sin(2.*M_PI*10.*systime));
    Convert_Fa_to_Shotnoise_Freqs() ;
    Fe += 5e-2;
    Fi += 4e-2;
    }
  else { Fa = 0 ; Fe = 0 ; Fi = 0 ; }
}
  
static DefaultGUIModel::variable_t vars[] =
  {
    { "Vm (V)", "Membrane potential (V)", DefaultGUIModel::INPUT, },
    { "Isyn (A)", "Output current (A)", DefaultGUIModel::OUTPUT, },
    { "LFP (V)", "Local Field Potential (V)", DefaultGUIModel::OUTPUT, },
    {"Fa (Hz)", "", DefaultGUIModel::STATE, },
    {"Fe (Hz)", "", DefaultGUIModel::STATE, },
    {"Fi (Hz)", "", DefaultGUIModel::STATE, },
  };

static size_t num_vars = sizeof(vars) / sizeof(DefaultGUIModel::variable_t);

InVivoLikeCorticalAct::InVivoLikeCorticalAct(void)
  : DefaultGUIModel("InVivo-Like Cortical Activity", ::vars, ::num_vars)
{
  setWhatsThis("<p><b>InVivoLikeCorticalAct:</b><br>Description.</p>");
  initParameters();
  DefaultGUIModel::createGUI(vars,
                             num_vars); // this is required to create the GUI
  customizeGUI();
  update(INIT); // this is optional, you may place initialization code directly
                // into the constructor
  refresh();    // this is required to update the GUI with parameter and state
                // values
  QTimer::singleShot(0, this, SLOT(resizeMe()));
}

InVivoLikeCorticalAct::~InVivoLikeCorticalAct(void)
{
}

void
InVivoLikeCorticalAct::execute(void)
{
  double Ie, Ii; // excitatory and inhibitory currents
  systime = count * period; // time in seconds
  Vm = input(0); // voltage in V
  Temporal_Dynamics_of_Freqs();
  for (int i = 0; i < steps; ++i)
    {
      conductance_update(&Ga, &Ge, &Gi) ;
    }
  /* === HERE WE GET BACK TO SI UNITS === */
  Ie = (Ga*1e-9)*(Ee*1e-3-Vm)+(Ge*1e-9)*(Ee*1e-3-Vm);
  Ii = (Gi*1e-9)*(Ei*1e-3-Vm);
  output(0) = Ie+Ii;
  output(1) = lfp_scaling*(Ii-Ie+10e-9*(-70e-3-Vm)); // LFP-like signal, reintroducing leak currents
  count++;
}

void
InVivoLikeCorticalAct::initParameters(void)
{
  Fa = 0; Fe = 0; Fi = 0;
  state_value = 0;
  Ga = 0.0; Ge = 0.0; Gi = 0.0; // nS
  Qa = 4.0; Qe = 2.0; Qi = 10.0; // nS
  Te = 5.0; Ti = 5.0; // ms
  Ee = 0.0; Ei = -80.0; // mV
  Na = 10; Ne = 200; Ni = 50;
  Vm = -70.0 ;
  systime = 0.0;
  count = 0;
  rate = 20000.0; // 20kHz per default here
  period = RT::System::getInstance()->getPeriod() * 1e-9; // s
  steps = static_cast<int> (ceil(period * rate));  
  lfp_scaling = 1e5 ;
}

void
InVivoLikeCorticalAct::update(DefaultGUIModel::update_flags_t flag)
{
  switch (flag) {
    case INIT:
      period = RT::System::getInstance()->getPeriod() * 1e-9; // s
      setState("Fa (Hz)", Fa);
      setState("Fe (Hz)", Fe);
      setState("Fi (Hz)", Fi);
      break;

    case MODIFY:
      Ga = 0.; Ge = 0; Gi = 0; // nS
      break;

    case UNPAUSE:
      break;

    case PAUSE:
      break;

    case PERIOD:
      period = RT::System::getInstance()->getPeriod() * 1e-9; // s
      steps = static_cast<int> (ceil(period * rate));
      break;

    default:
      break;
  }
}

void
InVivoLikeCorticalAct::customizeGUI(void)
{

  QGridLayout* customlayout = DefaultGUIModel::getLayout();
  QGroupBox* stateLayout = new QGroupBox(tr("Choose the Regime of Cortical Activity"));
  QHBoxLayout* stateGroup = new QHBoxLayout();

  stateGroup->addWidget(new QLabel(tr("Cortical State:")));
  typeList = new QComboBox;
  stateGroup->addWidget(typeList);
  typeList->addItem("Deep Anesthesia");
  typeList->addItem("Sleep-like Rhythm");
  typeList->addItem("Awake-like Act.");
  QObject::connect(typeList,SIGNAL(activated(int)),this,SLOT(Bttn_event()));
  stateLayout->setLayout(stateGroup);
  customlayout->addWidget(stateLayout, 0, 0);
}

void InVivoLikeCorticalAct::Bttn_event(void)
{
  if (typeList->currentText()=="Deep Anesthesia") state_value = 0;
  if (typeList->currentText()=="Sleep-like Rhythm") state_value = 1;
  if (typeList->currentText()=="Awake-like Act.") state_value = 2;
}

