// PrintableSum.h

#pragma once

#include "IPrintable.h"
#include "HorizontalLayout.h"

#include "layers.h"
using namespace layers;

/// A struct that adds printing capabilities to a layer
///
struct PrintableSum : public Sum, public IPrintable
{
public:
    /// Prints a human-firiendly description of the underlying class to an output stream
    ///
    virtual void Print(ostream& os) const override;

    /// Computes the layer layout
    ///
    virtual void ComputeLayout(const CommandLineArgs& args, double yOffset) override;

    /// Returns the begin-point of an arrow
    ///
    virtual Point GetBeginPoint(uint64 index) const override;

    /// Returns the end-point of an arrow
    ///
    virtual Point GetEndPoint(uint64 index) const override;

    /// \returns the layer width
    ///
    virtual double GetWidth() const override;

    /// \returns the layer height
    ///
    virtual double GetHeight() const override;

    /// \returns True if the specified element is visible
    ///
    virtual bool IsHidden(uint64 index) const override;

    /// \returns the layer type name
    ///
    virtual string GetTypeName() const override;

private:
    double _elementLeft;
    double _elementTop;
    double _elementXMid;
    double _elementYMid;
};