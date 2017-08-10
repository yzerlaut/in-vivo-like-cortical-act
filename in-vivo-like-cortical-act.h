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
 * This is a template header file for a user modules derived from
 * DefaultGUIModel with a custom GUI.
 */

#include <default_gui_model.h>

class InVivoLikeCorticalAct : public DefaultGUIModel
{

  Q_OBJECT

public:
  InVivoLikeCorticalAct(void);
  virtual ~InVivoLikeCorticalAct(void);

  void execute(void);
  void createGUI(DefaultGUIModel::variable_t*, int);
  void customizeGUI(void);

protected:
  virtual void update(DefaultGUIModel::update_flags_t);

private:
  
  QComboBox *typeList;
  
  double RdmNumber();
  void conductance_update(double *, double *, double *);
  void Convert_Fa_to_Shotnoise_Freqs();
  void Temporal_Dynamics_of_Freqs();
  
  double Vm;
  double Ga, Ge, Gi; /* synaptic conductances */
  
  double Fa, Fe, Fi;
  double state_value;
  double period, rate, systime;
  int steps;
  long long count;
  double Qa, Qe, Qi;
  double Te, Ti;
  double Ee, Ei;
  int Na, Ne, Ni;

  void initParameters();
  
private slots:
  // these are custom functions that can also be connected to events
  // through the Qt API. they must be implemented in plugin_template.cpp
  void Bttn_event(void);
};
