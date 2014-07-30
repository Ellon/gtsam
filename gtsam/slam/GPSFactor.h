/* ----------------------------------------------------------------------------

 * GTSAM Copyright 2010, Georgia Tech Research Corporation, 
 * Atlanta, Georgia 30332-0415
 * All Rights Reserved
 * Authors: Frank Dellaert, et al. (see THANKS for the full author list)

 * See LICENSE for the license information

 * -------------------------------------------------------------------------- */

/**
 *  @file  GPSFactor.h
 *  @author Luca Carlone
 **/
#pragma once

#include <ostream>

#include <gtsam/geometry/Pose3.h>
#include <gtsam/nonlinear/NonlinearFactor.h>

namespace gtsam {

  /**
   * A class to model GPS measurements, including a bias term which models
   * common-mode errors and that can be partially corrected if other sensors are used
   * @addtogroup SLAM
   */
  class GPSFactor: public NoiseModelFactor2<Pose3, Point3> {

  private:

    typedef GPSFactor This;
    typedef NoiseModelFactor2<Pose3, Point3> Base;

    Point3 measured_; /** The measurement */

  public:

    // shorthand for a smart pointer to a factor
    typedef typename boost::shared_ptr<GPSFactor> shared_ptr;

    /** default constructor - only use for serialization */
    GPSFactor() {}

    /** Constructor */
    GPSFactor(Key posekey, Key biaskey, const Point3 measured,
        const SharedNoiseModel& model) :
      Base(model, posekey, biaskey), measured_(measured) {
    }

    virtual ~GPSFactor() {}

    /** implement functions needed for Testable */

    /** print */
    virtual void print(const std::string& s, const KeyFormatter& keyFormatter = DefaultKeyFormatter) const {
      std::cout << s << "GPSFactor("
          << keyFormatter(this->key1()) << ","
          << keyFormatter(this->key2()) << ")\n";
      measured_.print("  measured: ");
      this->noiseModel_->print("  noise model: ");
    }

    /** equals */
    virtual bool equals(const NonlinearFactor& expected, double tol=1e-9) const {
      const This *e =  dynamic_cast<const This*> (&expected);
      return e != NULL && Base::equals(*e, tol) && this->measured_.equals(e->measured_, tol);
    }

    /** implement functions needed to derive from Factor */

    /** vector of errors */
    Vector evaluateError(const Pose3& pose, const Point3& bias,
        boost::optional<Matrix&> H1 = boost::none, boost::optional<Matrix&> H2 =
            boost::none) const {

      if (H1 || H2){
        H1->resize(3,6); // jacobian wrt pose
        (*H1) << Matrix3::Zero(),  pose.rotation().matrix();
        H2->resize(3,3); // jacobian wrt bias
        (*H2) << Matrix3::Identity();
      }
      return pose.translation().vector() + bias.vector() - measured_.vector();
    }

    /** return the measured */
    const Point3 measured() const {
      return measured_;
    }

  private:

    /** Serialization function */
    friend class boost::serialization::access;
    template<class ARCHIVE>
    void serialize(ARCHIVE & ar, const unsigned int version) {
      ar & boost::serialization::make_nvp("NoiseModelFactor2",
          boost::serialization::base_object<Base>(*this));
      ar & BOOST_SERIALIZATION_NVP(measured_);
    }
  }; // \class GPSFactor

} /// namespace gtsam
