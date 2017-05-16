/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/



#include "glwidget.h"
#include "window.h"
#include "vecpnt.h"
#include "common/str.h"
#include <QMessageBox>

#include <cmath>
#include <string>

using namespace std;

void Window::addSlider(QVBoxLayout * layout, const QString &label, QSlider * slider, float startValue, float scale, float low, float high, Transform sform)
{
    const float defaultValue = startValue;
    const int defaultScaled = int(std::round(defaultValue * scale));

    // specify slider parameters and label
    layout->addWidget(new QLabel(label));
    slider = new QSlider(Qt::Horizontal);
    slider->setMinimum(int(std::ceil(low * scale)));
    slider->setMaximum(int(std::floor(high * scale)));
    slider->setPageStep(200);
    slider->setSingleStep(1);
    slider->setTracking(true);
    slider->setValue(defaultScaled);

    const float invScale = 1.0f / scale;
    switch(sform)
    {
        case Transform::SCF: // scale
            connect(slider, &QSlider::valueChanged, [this, invScale] (int newValue)
                    {
                        perspectiveView->getXSect()->setScale(newValue * invScale);
                        perspectiveView->setMeshVisible(true);
                        repaintAllGL();
                    });
            break;
        case Transform::XTRS: // translation in x
            connect(slider, &QSlider::valueChanged, [this, invScale] (int newValue)
                    {
                        cgp::Vector trs = perspectiveView->getXSect()->getTranslation();
                        trs.i = newValue * invScale;
                        perspectiveView->getXSect()->setTranslation(trs);
                        perspectiveView->setMeshVisible(true);
                        repaintAllGL();
                    });
            break;
        case Transform::YTRS: // translation in y
            connect(slider, &QSlider::valueChanged, [this, invScale] (int newValue)
                    {
                        cgp::Vector trs = perspectiveView->getXSect()->getTranslation();
                        trs.j = newValue * invScale;
                        perspectiveView->getXSect()->setTranslation(trs);
                        perspectiveView->setMeshVisible(true);
                        repaintAllGL();
                    });
            break;
        case Transform::ZTRS: // translation in z
            connect(slider, &QSlider::valueChanged, [this, invScale] (int newValue)
                    {
                        cgp::Vector trs = perspectiveView->getXSect()->getTranslation();
                        trs.k = newValue * invScale;
                        perspectiveView->getXSect()->setTranslation(trs);
                        perspectiveView->setMeshVisible(true);
                        repaintAllGL();
                    });
            break;
        case Transform::XROT:
            connect(slider, &QSlider::valueChanged, [this, invScale] (int newValue)
                    {
                        float ax, ay, az;
                        perspectiveView->getXSect()->getRotations(ax, ay, az);
                        ax = newValue * invScale;
                        perspectiveView->getXSect()->setRotations(ax, ay, az);
                        perspectiveView->setMeshVisible(true);
                        repaintAllGL();
                    });
            break;
        case Transform::YROT:
            connect(slider, &QSlider::valueChanged, [this, invScale] (int newValue)
                    {
                        float ax, ay, az;
                        perspectiveView->getXSect()->getRotations(ax, ay, az);
                        ay = newValue * invScale;
                        perspectiveView->getXSect()->setRotations(ax, ay, az);
                        perspectiveView->setMeshVisible(true);
                        repaintAllGL();
                    });
            break;
        case Transform::ZROT:
            connect(slider, &QSlider::valueChanged, [this, invScale] (int newValue)
                    {
                        float ax, ay, az;
                        perspectiveView->getXSect()->getRotations(ax, ay, az);
                        az = newValue * invScale;
                        perspectiveView->getXSect()->setRotations(ax, ay, az);
                        perspectiveView->setMeshVisible(true);
                        repaintAllGL();
                    });
            break;
        default:
            break;
    }
    layout->addWidget(slider);
}

QSize Window::sizeHint() const
{
    return QSize(1152, 864);
}

Window::Window()
{
    QWidget *mainWidget = new QWidget;
    QGridLayout *mainLayout = new QGridLayout;

    setCentralWidget(mainWidget);
    mainLayout->setColumnStretch(0, 0);
    mainLayout->setColumnStretch(1, 1);

    // OpenGL widget
    // Specify an OpenGL 3.2 format.
    QGLFormat glFormat;
    glFormat.setVersion( 3, 2 );
    glFormat.setProfile( QGLFormat::CoreProfile );
    glFormat.setSampleBuffers( false );

    perspectiveView = new GLWidget(glFormat);

    getCamera().setForcedFocus(cgp::Point(0.0f, 0.0f, 0.0f));
    getCamera().setViewScale(1.0f);

    // param panel
    paramPanel = new QWidget;
    QVBoxLayout *paramLayout = new QVBoxLayout;

    // components on parameter panel

    // mesh settings
    QGroupBox *meshGroup = new QGroupBox(tr("Mesh Settings"));

    float ax, ay, az;
    perspectiveView->getXSect()->getRotations(ax, ay, az);

    QVBoxLayout *meshLayout = new QVBoxLayout;
    addSlider(meshLayout, tr("Scale"), scfslider, 1.0f, 20.0f, 0.0f, 5.0f, Transform::SCF);
    addSlider(meshLayout, tr("Trs X"), xtrslider, 0.0f, 20.0f, -10.0f, 10.0f, Transform::XTRS);
    addSlider(meshLayout, tr("Trs Y"), ytrslider, 0.0f, 20.0f, -10.0f, 10.0f, Transform::YTRS);
    addSlider(meshLayout, tr("Trs Z"), ztrslider, 0.0f, 20.0f, -10.0f, 10.0f, Transform::ZTRS);
    addSlider(meshLayout, tr("Rot X"), xrotslider, 0.0f, 1.0f, 0.0f, 360.0f, Transform::XROT);
    addSlider(meshLayout, tr("Rot Y"), yrotslider, 0.0f, 1.0f, 0.0f, 360.0f, Transform::YROT);
    addSlider(meshLayout, tr("Rot Z"), zrotslider, 0.0f, 1.0f, 0.0f, 360.0f, Transform::ZROT);

    meshGroup->setLayout(meshLayout);
    paramLayout->addWidget(meshGroup);

    // check box for display of intersect model
    checkModel = new QCheckBox(tr("Show Intersector Model"));
    checkModel->setChecked(false);
    paramLayout->addWidget(checkModel);

    // signal to slot connections
    connect(perspectiveView, SIGNAL(signalRepaintAllGL()), this, SLOT(repaintAllGL()));
    connect(checkModel, SIGNAL(stateChanged(int)), this, SLOT(showModel(int)));

    paramPanel->setLayout(paramLayout);
    mainLayout->addWidget(perspectiveView, 0, 1);
    mainLayout->addWidget(paramPanel, 0, 0, Qt::AlignTop);

    createActions();
    createMenus();

    mainWidget->setLayout(mainLayout);
    setWindowTitle(tr("Tesselation Viewer"));
    mainWidget->setMouseTracking(true);
    setMouseTracking(true);
}

void Window::keyPressEvent(QKeyEvent *e)
{
    // pass to render window
    perspectiveView->keyPressEvent(e);
}

void Window::mouseMoveEvent(QMouseEvent *event)
{
    QWidget *child=childAt(event->pos());
    QGLWidget *glwidget = qobject_cast<QGLWidget *>(child);
    if(glwidget) {
        QMouseEvent *glevent=new QMouseEvent(event->type(),glwidget->mapFromGlobal(event->globalPos()),event->button(),event->buttons(),event->modifiers());
        QCoreApplication::postEvent(glwidget,glevent);
    }
}

void Window::repaintAllGL()
{
    perspectiveView->repaint();
}

void Window::newFile()
{
    // clear everything and reset
    // this does not reset the volume parameters
    perspectiveView->getXSect()->clear();
    perspectiveView->setMeshVisible(false);
    perspectiveView->setGeometryUpdate(true);
}

void Window::open()
{
    QFileDialog::Options options;
    QString selectedFilter;
    QImage cap;

    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open Intersection File"),
                                                    "~/",
                                                    tr("STL Files (*.stl)"),
                                                    &selectedFilter,
                                                    options);
    if (!fileName.isEmpty())
    {
        std::string infile = fileName.toUtf8().constData();

        // use file extension to determine action
        if(endsWith(infile, ".stl"))
        {
            perspectiveView->getXSect()->readSTL(infile);
            perspectiveView->getXSect()->boxFit(10.0f);

            perspectiveView->setMeshVisible(true);
            checkModel->setChecked(true);
            repaintAllGL();
        }
        else
        {
            cerr << "Error Window::open: attempt to open unrecognized file format" << endl;
        }
    }
}

void Window::saveFile()
{
    if(!tessfilename.isEmpty()) // save directly if we already have a file name
    {
        std::string outfile = tessfilename.toUtf8().constData();
        if(!endsWith(outfile, ".stl"))
            outfile = outfile + ".stl";
        if(!perspectiveView->getXSect()->writeSTL(outfile)) // error message
        {
            QMessageBox msgBox;
            msgBox.setText("Unable to save mesh to file");
            msgBox.exec();
        }
    }
    else
    {
        saveAs();
    }
}

void Window::saveAs()
{
    QFileDialog::Options options;
    QString selectedFilter;
    tessfilename = QFileDialog::getSaveFileName(this,
                                                tr("Save Tesselation"),
                                                "~/",
                                                tr("STL File (*.stl)"),
                                                &selectedFilter,
                                                options);
    if (!tessfilename.isEmpty())
    {
        std::string outfile = tessfilename.toUtf8().constData();
        if(!endsWith(outfile, ".stl"))
            outfile = outfile + ".stl";
        if(!perspectiveView->getXSect()->writeSTL(outfile)) // error message
        {
            QMessageBox msgBox;
            msgBox.setText("Unable to save mesh to file");
            msgBox.exec();
        }
    }
}

void Window::showModel(int show)
{
    perspectiveView->setMeshVisible(show == Qt::Checked);
    repaintAllGL();
}

void Window::showParamOptions()
{
    paramPanel->setVisible(showParamAct->isChecked());
}

void Window::createActions()
{
    newAct = new QAction(tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));

    openAct = new QAction(tr("&Open"), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    saveAct = new QAction(tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save a file"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(saveFile()));

    saveAsAct = new QAction(tr("Save as"), this);
    saveAsAct->setStatusTip(tr("Save a file under name"));
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

    showParamAct = new QAction(tr("Show Parameters"), this);
    showParamAct->setCheckable(true);
    showParamAct->setChecked(true);
    showParamAct->setStatusTip(tr("Hide/Show Parameters"));
    connect(showParamAct, SIGNAL(triggered()), this, SLOT(showParamOptions()));
}

void Window::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(showParamAct);
}
