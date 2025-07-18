/*
 * Copyright (c) 2013-2025 Meltytech, LLC
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef COMMANDS_H
#define COMMANDS_H

#include "docks/timelinedock.h"
#include "models/markersmodel.h"
#include "models/multitrackmodel.h"
#include "undohelper.h"

#include <MltProducer.h>
#include <MltTransition.h>
#include <QObject>
#include <QString>
#include <QUndoCommand>
#include <QUuid>

#include <vector>

namespace Timeline {

enum {
    UndoIdTrimClipIn = 100,
    UndoIdTrimClipOut,
    UndoIdFadeIn,
    UndoIdFadeOut,
    UndoIdTrimTransitionIn,
    UndoIdTrimTransitionOut,
    UndoIdAddTransitionByTrimIn,
    UndoIdAddTransitionByTrimOut,
    UndoIdUpdate,
    UndoIdMoveClip,
    UndoIdChangeGain,
};

struct ClipPosition
{
    ClipPosition(int track, int clip)
    {
        trackIndex = track;
        clipIndex = clip;
    }

    bool operator<(const ClipPosition &rhs) const
    {
        if (trackIndex == rhs.trackIndex) {
            return clipIndex < rhs.clipIndex;
        } else {
            return trackIndex < rhs.trackIndex;
        }
    }

    int trackIndex;
    int clipIndex;
};

class AppendCommand : public QUndoCommand
{
public:
    AppendCommand(MultitrackModel &model,
                  int trackIndex,
                  const QString &xml,
                  bool skipProxy = false,
                  bool seek = true,
                  QUndoCommand *parent = 0);
    void redo();
    void undo();

private:
    MultitrackModel &m_model;
    int m_trackIndex;
    QString m_xml;
    UndoHelper m_undoHelper;
    bool m_skipProxy;
    bool m_seek;
    QVector<QUuid> m_uuids;
};

class InsertCommand : public QUndoCommand
{
public:
    InsertCommand(MultitrackModel &model,
                  MarkersModel &markersModel,
                  int trackIndex,
                  int position,
                  const QString &xml,
                  bool seek = true,
                  QUndoCommand *parent = 0);
    void redo();
    void undo();

private:
    MultitrackModel &m_model;
    MarkersModel &m_markersModel;
    int m_trackIndex;
    int m_position;
    QString m_xml;
    QStringList m_oldTracks;
    UndoHelper m_undoHelper;
    bool m_seek;
    bool m_rippleAllTracks;
    bool m_rippleMarkers;
    int m_markersShift;
    QVector<QUuid> m_uuids;
};

class OverwriteCommand : public QUndoCommand
{
public:
    OverwriteCommand(MultitrackModel &model,
                     int trackIndex,
                     int position,
                     const QString &xml,
                     bool seek = true,
                     QUndoCommand *parent = 0);
    void redo();
    void undo();

private:
    MultitrackModel &m_model;
    int m_trackIndex;
    int m_position;
    QString m_xml;
    UndoHelper m_undoHelper;
    bool m_seek;
    QVector<QUuid> m_uuids;
};

class LiftCommand : public QUndoCommand
{
public:
    LiftCommand(MultitrackModel &model, int trackIndex, int clipIndex, QUndoCommand *parent = 0);
    void redo();
    void undo();

private:
    MultitrackModel &m_model;
    int m_trackIndex;
    int m_clipIndex;
    UndoHelper m_undoHelper;
};

class RemoveCommand : public QUndoCommand
{
public:
    RemoveCommand(MultitrackModel &model,
                  MarkersModel &markersModel,
                  int trackIndex,
                  int clipIndex,
                  QUndoCommand *parent = 0);
    void redo();
    void undo();

private:
    MultitrackModel &m_model;
    MarkersModel &m_markersModel;
    int m_trackIndex;
    int m_clipIndex;
    UndoHelper m_undoHelper;
    bool m_rippleAllTracks;
    bool m_rippleMarkers;
    int m_markerRemoveStart;
    int m_markerRemoveEnd;
    QList<Markers::Marker> m_markers;
};

class GroupCommand : public QUndoCommand
{
public:
    GroupCommand(MultitrackModel &model, QUndoCommand *parent = 0);
    void addToGroup(int trackIndex, int clipIndex);
    void redo();
    void undo();

private:
    MultitrackModel &m_model;
    QList<ClipPosition> m_clips;
    QMap<ClipPosition, int> m_prevGroups;
};

class UngroupCommand : public QUndoCommand
{
public:
    UngroupCommand(MultitrackModel &model, QUndoCommand *parent = 0);
    void removeFromGroup(int trackIndex, int clipIndex);
    void redo();
    void undo();

private:
    MultitrackModel &m_model;
    QMap<ClipPosition, int> m_prevGroups;
};

class NameTrackCommand : public QUndoCommand
{
public:
    NameTrackCommand(MultitrackModel &model,
                     int trackIndex,
                     const QString &name,
                     QUndoCommand *parent = 0);
    void redo();
    void undo();

private:
    MultitrackModel &m_model;
    int m_trackIndex;
    QString m_name;
    QString m_oldName;
};

class MergeCommand : public QUndoCommand
{
public:
    MergeCommand(MultitrackModel &model, int trackIndex, int clipIndex, QUndoCommand *parent = 0);
    void redo();
    void undo();

private:
    MultitrackModel &m_model;
    int m_trackIndex;
    int m_clipIndex;
    UndoHelper m_undoHelper;
};

class MuteTrackCommand : public QUndoCommand
{
public:
    MuteTrackCommand(MultitrackModel &model, int trackIndex, QUndoCommand *parent = 0);
    void redo();
    void undo();

private:
    MultitrackModel &m_model;
    int m_trackIndex;
    bool m_oldValue;
};

class HideTrackCommand : public QUndoCommand
{
public:
    HideTrackCommand(MultitrackModel &model, int trackIndex, QUndoCommand *parent = 0);
    void redo();
    void undo();

private:
    MultitrackModel &m_model;
    int m_trackIndex;
    bool m_oldValue;
};

class CompositeTrackCommand : public QUndoCommand
{
public:
    CompositeTrackCommand(MultitrackModel &model,
                          int trackIndex,
                          bool value,
                          QUndoCommand *parent = 0);
    void redo();
    void undo();

private:
    MultitrackModel &m_model;
    int m_trackIndex;
    bool m_value;
    bool m_oldValue;
};

class LockTrackCommand : public QUndoCommand
{
public:
    LockTrackCommand(MultitrackModel &model, int trackIndex, bool value, QUndoCommand *parent = 0);
    void redo();
    void undo();

private:
    MultitrackModel &m_model;
    int m_trackIndex;
    bool m_value;
    bool m_oldValue;
};

class MoveClipCommand : public QUndoCommand
{
public:
    MoveClipCommand(TimelineDock &timeline,
                    int trackDelta,
                    int positionDelta,
                    bool ripple,
                    QUndoCommand *parent = 0);
    void addClip(int trackIndex, int clipIndex);
    void redo();
    void undo();

protected:
    int id() const { return UndoIdMoveClip; }
    bool mergeWith(const QUndoCommand *other);

private:
    void redoMarkers();

    TimelineDock &m_timeline;
    MultitrackModel &m_model;
    MarkersModel &m_markersModel;

    struct Info
    {
        int trackIndex;
        int clipIndex;
        int frame_in;
        int frame_out;
        int start;
        int group;
        QUuid uuid;

        Info()
            : trackIndex(-1)
            , clipIndex(-1)
            , frame_in(-1)
            , frame_out(-1)
            , start(0)
            , group(-1)
        {}
    };

    int m_trackDelta;
    int m_positionDelta;
    bool m_ripple;
    bool m_rippleAllTracks;
    bool m_rippleMarkers;
    UndoHelper m_undoHelper;
    QMultiMap<int, Info> m_clips; // ordered by position
    bool m_redo;
    int m_earliestStart;
    QList<Markers::Marker> m_markers;
};

class TrimCommand : public QUndoCommand
{
public:
    explicit TrimCommand(QUndoCommand *parent = 0)
        : QUndoCommand(parent)
    {}
    void setUndoHelper(UndoHelper *helper) { m_undoHelper.reset(helper); }

protected:
    QScopedPointer<UndoHelper> m_undoHelper;
};

class TrimClipInCommand : public TrimCommand
{
public:
    TrimClipInCommand(MultitrackModel &model,
                      MarkersModel &markersModel,
                      int trackIndex,
                      int clipIndex,
                      int delta,
                      bool ripple,
                      bool redo = true,
                      QUndoCommand *parent = 0);
    void redo();
    void undo();

protected:
    int id() const { return UndoIdTrimClipIn; }
    bool mergeWith(const QUndoCommand *other);

private:
    MultitrackModel &m_model;
    MarkersModel &m_markersModel;
    int m_trackIndex;
    int m_clipIndex;
    int m_delta;
    bool m_ripple;
    bool m_rippleAllTracks;
    bool m_rippleMarkers;
    bool m_redo;
    int m_markerRemoveStart;
    int m_markerRemoveEnd;
    QList<Markers::Marker> m_markers;
};

class TrimClipOutCommand : public TrimCommand
{
public:
    TrimClipOutCommand(MultitrackModel &model,
                       MarkersModel &markersModel,
                       int trackIndex,
                       int clipIndex,
                       int delta,
                       bool ripple,
                       bool redo = true,
                       QUndoCommand *parent = 0);
    void redo();
    void undo();

protected:
    int id() const { return UndoIdTrimClipOut; }
    bool mergeWith(const QUndoCommand *other);

private:
    MultitrackModel &m_model;
    MarkersModel &m_markersModel;
    int m_trackIndex;
    int m_clipIndex;
    int m_delta;
    bool m_ripple;
    bool m_rippleAllTracks;
    bool m_rippleMarkers;
    bool m_redo;
    int m_markerRemoveStart;
    int m_markerRemoveEnd;
    QList<Markers::Marker> m_markers;
};

class SplitCommand : public QUndoCommand
{
public:
    SplitCommand(MultitrackModel &model,
                 const std::vector<int> &trackIndex,
                 const std::vector<int> &clipIndex,
                 int position,
                 QUndoCommand *parent = 0);
    void redo();
    void undo();

private:
    MultitrackModel &m_model;
    std::vector<int> m_trackIndex;
    std::vector<int> m_clipIndex;
    int m_position;
    UndoHelper m_undoHelper;
};

class FadeInCommand : public QUndoCommand
{
public:
    FadeInCommand(MultitrackModel &model,
                  int trackIndex,
                  int clipIndex,
                  int duration,
                  QUndoCommand *parent = 0);
    void redo();
    void undo();

protected:
    int id() const { return UndoIdFadeIn; }
    bool mergeWith(const QUndoCommand *other);

private:
    MultitrackModel &m_model;
    int m_trackIndex;
    int m_clipIndex;
    int m_duration;
    int m_previous;
};

class FadeOutCommand : public QUndoCommand
{
public:
    FadeOutCommand(MultitrackModel &model,
                   int trackIndex,
                   int clipIndex,
                   int duration,
                   QUndoCommand *parent = 0);
    void redo();
    void undo();

protected:
    int id() const { return UndoIdFadeOut; }
    bool mergeWith(const QUndoCommand *other);

private:
    MultitrackModel &m_model;
    int m_trackIndex;
    int m_clipIndex;
    int m_duration;
    int m_previous;
};

class AddTransitionCommand : public QUndoCommand
{
public:
    AddTransitionCommand(TimelineDock &timeline,
                         int trackIndex,
                         int clipIndex,
                         int position,
                         bool ripple,
                         QUndoCommand *parent = 0);
    void redo();
    void undo();
    int getTransitionIndex() const { return m_transitionIndex; }

private:
    TimelineDock &m_timeline;
    MultitrackModel &m_model;
    MarkersModel &m_markersModel;
    int m_trackIndex;
    int m_clipIndex;
    int m_position;
    int m_transitionIndex;
    bool m_ripple;
    UndoHelper m_undoHelper;
    bool m_rippleAllTracks;
    bool m_rippleMarkers;
    int m_markerOldStart;
    int m_markerNewStart;
    QList<Markers::Marker> m_markers;
};

class TrimTransitionInCommand : public TrimCommand
{
public:
    TrimTransitionInCommand(MultitrackModel &model,
                            int trackIndex,
                            int clipIndex,
                            int delta,
                            bool redo = true,
                            QUndoCommand *parent = 0);
    void redo();
    void undo();

protected:
    int id() const { return UndoIdTrimTransitionIn; }
    bool mergeWith(const QUndoCommand *other);

private:
    MultitrackModel &m_model;
    int m_trackIndex;
    int m_clipIndex;
    int m_delta;
    bool m_notify;
    bool m_redo;
};

class TrimTransitionOutCommand : public TrimCommand
{
public:
    TrimTransitionOutCommand(MultitrackModel &model,
                             int trackIndex,
                             int clipIndex,
                             int delta,
                             bool redo = true,
                             QUndoCommand *parent = 0);
    void redo();
    void undo();

protected:
    int id() const { return UndoIdTrimTransitionOut; }
    bool mergeWith(const QUndoCommand *other);

private:
    MultitrackModel &m_model;
    int m_trackIndex;
    int m_clipIndex;
    int m_delta;
    bool m_notify;
    bool m_redo;
};

class AddTransitionByTrimInCommand : public TrimCommand
{
public:
    AddTransitionByTrimInCommand(TimelineDock &timeline,
                                 int trackIndex,
                                 int clipIndex,
                                 int duration,
                                 int trimDelta,
                                 bool redo = true,
                                 QUndoCommand *parent = 0);
    void redo();
    void undo();

protected:
    int id() const { return UndoIdAddTransitionByTrimIn; }
    bool mergeWith(const QUndoCommand *other);

private:
    TimelineDock &m_timeline;
    int m_trackIndex;
    int m_clipIndex;
    int m_duration;
    int m_trimDelta;
    bool m_notify;
    bool m_redo;
};

class RemoveTransitionByTrimInCommand : public TrimCommand
{
public:
    RemoveTransitionByTrimInCommand(MultitrackModel &model,
                                    int trackIndex,
                                    int clipIndex,
                                    int delta,
                                    QString xml,
                                    bool redo = true,
                                    QUndoCommand *parent = 0);
    void redo();
    void undo();

private:
    MultitrackModel &m_model;
    int m_trackIndex;
    int m_clipIndex;
    int m_delta;
    QString m_xml;
    bool m_redo;
};

class RemoveTransitionByTrimOutCommand : public TrimCommand
{
public:
    RemoveTransitionByTrimOutCommand(MultitrackModel &model,
                                     int trackIndex,
                                     int clipIndex,
                                     int delta,
                                     QString xml,
                                     bool redo = true,
                                     QUndoCommand *parent = 0);
    void redo();
    void undo();

private:
    MultitrackModel &m_model;
    int m_trackIndex;
    int m_clipIndex;
    int m_delta;
    QString m_xml;
    bool m_redo;
};

class AddTransitionByTrimOutCommand : public TrimCommand
{
public:
    AddTransitionByTrimOutCommand(MultitrackModel &model,
                                  int trackIndex,
                                  int clipIndex,
                                  int duration,
                                  int trimDelta,
                                  bool redo = true,
                                  QUndoCommand *parent = 0);
    void redo();
    void undo();

protected:
    int id() const { return UndoIdAddTransitionByTrimOut; }
    bool mergeWith(const QUndoCommand *other);

private:
    MultitrackModel &m_model;
    int m_trackIndex;
    int m_clipIndex;
    int m_duration;
    int m_trimDelta;
    bool m_notify;
    bool m_redo;
};

class AddTrackCommand : public QUndoCommand
{
public:
    AddTrackCommand(MultitrackModel &model, bool isVideo, QUndoCommand *parent = 0);
    void redo();
    void undo();

private:
    MultitrackModel &m_model;
    int m_trackIndex;
    bool m_isVideo;
    QUuid m_uuid;
};

class InsertTrackCommand : public QUndoCommand
{
public:
    InsertTrackCommand(MultitrackModel &model,
                       int trackIndex,
                       TrackType trackType = PlaylistTrackType,
                       QUndoCommand *parent = 0);
    void redo();
    void undo();

private:
    MultitrackModel &m_model;
    int m_trackIndex;
    TrackType m_trackType;
    QUuid m_uuid;
};

class RemoveTrackCommand : public QUndoCommand
{
public:
    RemoveTrackCommand(MultitrackModel &model, int trackIndex, QUndoCommand *parent = 0);
    void redo();
    void undo();

private:
    MultitrackModel &m_model;
    int m_trackIndex;
    TrackType m_trackType;
    QString m_trackName;
    UndoHelper m_undoHelper;
    QScopedPointer<Mlt::Producer> m_filtersProducer;
    QUuid m_uuid;
};

class MoveTrackCommand : public QUndoCommand
{
public:
    MoveTrackCommand(MultitrackModel &model,
                     int fromTrackIndex,
                     int toTrackIndex,
                     QUndoCommand *parent = 0);
    void redo();
    void undo();

private:
    MultitrackModel &m_model;
    int m_fromTrackIndex;
    int m_toTrackIndex;
};

class ChangeBlendModeCommand : public QObject, public QUndoCommand
{
    Q_OBJECT
public:
    ChangeBlendModeCommand(Mlt::Transition &transition,
                           const QString &propertyName,
                           const QString &mode,
                           QUndoCommand *parent = 0);
    void redo();
    void undo();
signals:
    void modeChanged(QString &mode);

private:
    Mlt::Transition m_transition;
    QString m_propertyName;
    QString m_newMode;
    QString m_oldMode;
};

class UpdateCommand : public QUndoCommand
{
public:
    UpdateCommand(TimelineDock &timeline,
                  int trackIndex,
                  int clipIndex,
                  int position,
                  QUndoCommand *parent = 0);
    void setXmlAfter(const QString &xml);
    void setPosition(int trackIndex, int clipIndex, int position);
    void setRippleAllTracks(bool);
    int trackIndex() const { return m_trackIndex; }
    int clipIndex() const { return m_clipIndex; }
    int position() const { return m_position; }
    void redo();
    void undo();

private:
    TimelineDock &m_timeline;
    int m_trackIndex;
    int m_clipIndex;
    int m_position;
    QString m_xmlAfter;
    bool m_isFirstRedo;
    UndoHelper m_undoHelper;
    bool m_ripple;
    bool m_rippleAllTracks;
};

class DetachAudioCommand : public QUndoCommand
{
public:
    DetachAudioCommand(TimelineDock &timeline,
                       int trackIndex,
                       int clipIndex,
                       int position,
                       const QString &xml,
                       QUndoCommand *parent = 0);
    void redo();
    void undo();

private:
    TimelineDock &m_timeline;
    int m_trackIndex;
    int m_clipIndex;
    int m_position;
    int m_targetTrackIndex;
    QString m_xml;
    UndoHelper m_undoHelper;
    bool m_trackAdded;
    QUuid m_uuid;
};

class ReplaceCommand : public QUndoCommand
{
public:
    ReplaceCommand(MultitrackModel &model,
                   int trackIndex,
                   int clipIndex,
                   const QString &xml,
                   QUndoCommand *parent = nullptr);
    void redo();
    void undo();

private:
    MultitrackModel &m_model;
    int m_trackIndex;
    int m_clipIndex;
    QString m_xml;
    bool m_isFirstRedo;
    UndoHelper m_undoHelper;
};

class AlignClipsCommand : public QUndoCommand
{
public:
    AlignClipsCommand(MultitrackModel &model, QUndoCommand *parent = 0);
    void addAlignment(QUuid uuid, int offset, double speedCompensation);
    void redo();
    void undo();

private:
    MultitrackModel &m_model;
    UndoHelper m_undoHelper;
    bool m_redo;
    struct Alignment
    {
        QUuid uuid;
        int offset;
        double speed;
    };
    QVector<Alignment> m_alignments;
};

class ApplyFiltersCommand : public QUndoCommand
{
public:
    ApplyFiltersCommand(MultitrackModel &model,
                        const QString &filterProducerXml,
                        QUndoCommand *parent = 0);
    void addClip(int trackIndex, int clipIndex);
    void redo();
    void undo();

private:
    MultitrackModel &m_model;
    QString m_xml;
    QMap<ClipPosition, QString> m_prevFilters;
};

class ChangeGainCommand : public QUndoCommand
{
public:
    ChangeGainCommand(MultitrackModel &model,
                      int trackIndex,
                      int clipIndex,
                      double gain,
                      QUndoCommand *parent = 0);
    void redo();
    void undo();

protected:
    int id() const { return UndoIdChangeGain; }
    bool mergeWith(const QUndoCommand *other);

private:
    MultitrackModel &m_model;
    int m_trackIndex;
    int m_clipIndex;
    double m_gain;
    double m_previous;
};

} // namespace Timeline

#endif
